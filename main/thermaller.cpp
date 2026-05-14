#include <cstdint>
#include <esp_timer.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_log.h>

#include <driver/i2c_master.h>

#include "thermaller.h"
#include "config.h"

namespace pizda {
	using namespace YOBA;

	Thermaller& Thermaller::getInstance() {
		static auto instance = Thermaller();

		return instance;
	}

	[[noreturn]] void Thermaller::start() {
		// -------------------------------- Splash screen --------------------------------

		// First, let's render a splash screen while we wait for the peripherals to finish warming up

		// Multicore
		{
			_SPIMutex = xSemaphoreCreateMutex();
			system::SPI::setMutex(_SPIMutex);
		}

		// SPI
		{
			spi_bus_config_t config {};
			config.mosi_io_num = config::SPI::MOSI;
			config.miso_io_num = config::SPI::MISO;
			config.sclk_io_num = config::SPI::SCK;
			config.quadwp_io_num = -1;
			config.quadhd_io_num = -1;
			config.max_transfer_sz = _display.getSize().getSquare() * 2;

			ESP_ERROR_CHECK(spi_bus_initialize(config::SPI::device, &config, SPI_DMA_CH_AUTO));
		}

		// GPIO
		{
			// Slave selects
			gpio_config_t g = {};
			g.pin_bit_mask = (1ULL << config::screen::SS) | (1ULL << config::XCVR::SS);
			g.mode = GPIO_MODE_OUTPUT;
			g.pull_up_en = GPIO_PULLUP_DISABLE;
			g.pull_down_en = GPIO_PULLDOWN_DISABLE;
			g.intr_type = GPIO_INTR_DISABLE;
			gpio_config(&g);

			gpio_set_level(config::screen::SS, true);
			gpio_set_level(config::XCVR::SS, true);
		}

		// After applying power or a hard reset, the LCD panel will be turned off, its internal pixel
		// buffer will contain random garbage, and the driver will wait for pixel data to be received
		//
		// If you turn on the LCD before sending pixels, this garbage will be immediately shown,
		// which will definitely cause bleed from videophiles eyes
		//
		// So...
		_display.setup();
		_renderer.setTarget(&_display);

		// Rendering splash screen
		Theme::setup(&_renderer);
		_renderer.clear(&Theme::bg1);
		_renderer.renderImage(Point(), &resources::images::splashScreen);
		_renderer.flush();

		// Turning display on
		_display.turnOn();

		// -------------------------------- Hardware --------------------------------

		// NVS is required by settings & Wi-Fi
		{
			const auto status = nvs_flash_init();

			if (status == ESP_ERR_NVS_NO_FREE_PAGES || status == ESP_ERR_NVS_NEW_VERSION_FOUND) {
				// NVS partition was truncated and needs to be erased
				ESP_ERROR_CHECK(nvs_flash_erase());
				// Retry init
				ESP_ERROR_CHECK(nvs_flash_init());
			}
			else {
				ESP_ERROR_CHECK(status);
			}
		}

		// ADC
		{
			adc_oneshot_unit_init_cfg_t unitConfig {};
			unitConfig.unit_id = ADC_UNIT_1;
			unitConfig.clk_src = ADC_RTC_CLK_SThermaller_DEFAULT;
			unitConfig.ulp_mode = ADC_ULP_MODE_DISABLE;
			ESP_ERROR_CHECK(adc_oneshot_new_unit(&unitConfig, &_ADCOneshotUnit1));
		}

		// Settings come first because they contain XCVR modulation params, ADC axes calibration data, etc.
		_settings.readAll();

		// Transceiver
		if (!_transceiver.setup())
			startErrorLoop("failed to setup XCVR");

		// Touch panel
		_touchPanel.setup();
		_application.addHID(&_touchPanel);

		// Encoder
		_encoder.setup();
		_encoder.setMinimumDelta(4);
		_application.addHID(&_encoder);

		// Other shit
		_axes.setup();
		_battery.setup();
		_audioPlayer.setup();

		// -------------------------------- UI --------------------------------

		_application.setRenderer(&_renderer);
		_application.setBackgroundColor(&Theme::bg1);
		_application += &_pageLayout;
		_application += &_openMenuButton;

		setRoute(&Routes::MFD);
		updateDebugOverlayVisibility();

		// -------------------------------- Main loop --------------------------------

		_audioPlayer.play(&resources::sounds::boot);

		// This shit is blazingly 🔥 fast 🚀, so letting user enjoy logo for a few moments
		vTaskDelay(pdMS_TO_TICKS(500));

		while (true) {
			_axes.tick();
			batteryTick();

			interpolateData();

			_application.tick();
			_application.render();

			vTaskDelay(pdMS_TO_TICKS(1'000 / config::application::interfaceTickRateHz));
		}
	}

	float Thermaller::applyLPF(const float oldValue, const float newValue, const float factor) const {
		return
			_settings.personalization.LPF
			? LowPassFilter::apply(oldValue, newValue, factor)
			: newValue;
	}
	
	float Thermaller::applyLPFToAngle(const float oldValue, const float newValue, const float factor) const {
		return
			_settings.personalization.LPF
			? LowPassFilter::applyToAngle(oldValue, newValue, factor)
			: newValue;
	}
	
	void Thermaller::interpolateData() {
		const auto deltaTimeUs = esp_timer_get_time() - _dataInterpolationTime;
		_dataInterpolationTime = esp_timer_get_time();

		// Principle of calculating the interpolation factor:
		//
		// factorPerSecond -> 1'000'000 us
		// factorPerTick -> deltaTime us
		//
		// factorPerTick = factorPerSecond * deltaTime / 1'000'000

		// Fast
		float LPFFactor = LowPassFilter::getDeltaTimeUsFactor(5.0f, deltaTimeUs);
		
		// Pitch
		_aircraftData.computed.pitchRad = applyLPFToAngle(
			_aircraftData.computed.pitchRad,
			_aircraftData.raw.pitchRad,
			LPFFactor
		);
		
		// Roll
		_aircraftData.computed.rollRad = applyLPFToAngle(
			_aircraftData.computed.rollRad,
			_aircraftData.raw.rollRad,
			LPFFactor
		);
		
		// Yaw
		_aircraftData.computed.yawRad = applyLPFToAngle(
			_aircraftData.computed.yawRad,
			_aircraftData.raw.yawRad,
			LPFFactor
		);
		
		// Heading
		_aircraftData.computed.headingDeg = normalizeAngleDeg360(toDegrees(-_aircraftData.computed.yawRad));
//		ESP_LOGI("PIZDA", "raw: %f, raw deg: %f, computed: %f, deg: %f, heading: %f", _aircraftData.raw.yawRad, toDegrees(-_aircraftData.raw.yawRad), _aircraftData.computed.yawRad, toDegrees(_aircraftData.computed.yawRad), _aircraftData.computed.headingDeg);

		// Coordinates
		_aircraftData.computed.coordinates.setLatitude(applyLPF(
			_aircraftData.computed.coordinates.getLatitude(),
			_aircraftData.raw.coordinates.getLatitude(),
			LPFFactor
		));

		_aircraftData.computed.coordinates.setLongitude(applyLPF(
			_aircraftData.computed.coordinates.getLongitude(),
			_aircraftData.raw.coordinates.getLongitude(),
			LPFFactor
		));

		_aircraftData.computed.coordinates.setAltitude(applyLPF(
			_aircraftData.computed.coordinates.getAltitude(),
			_aircraftData.raw.coordinates.getAltitude(),
			LPFFactor
		));

		// Slip & skid
		_aircraftData.computed.slipAndSkidFactor = applyLPF(
			_aircraftData.computed.slipAndSkidFactor,
			_aircraftData.raw.slipAndSkidFactor,
			LPFFactor
		);

		// Flight path vector
		_aircraftData.computed.flightPathVectorPitchRad = applyLPF(
			_aircraftData.computed.flightPathVectorPitchRad,
			_aircraftData.raw.flightPathVectorPitchRad,
			LPFFactor
		);
		
		_aircraftData.computed.flightPathVectorYawRad = applyLPF(
			_aircraftData.computed.flightPathVectorYawRad,
			_aircraftData.raw.flightPathVectorYawRad,
			LPFFactor
		);
		
		// Flight director
		_aircraftData.computed.autopilot.rollRad = applyLPF(
			_aircraftData.computed.autopilot.rollRad,
			_aircraftData.raw.autopilot.rollRad,
			LPFFactor
		);
		
		_aircraftData.computed.autopilot.pitchRad = applyLPF(
			_aircraftData.computed.autopilot.pitchRad,
			_aircraftData.raw.autopilot.pitchRad,
			LPFFactor
		);
		
		// Normal
		LPFFactor = LowPassFilter::getDeltaTimeUsFactor(3.0f, deltaTimeUs);
		
		// Air speed
		_aircraftData.computed.airspeedKt = applyLPF(
			_aircraftData.computed.airspeedKt,
			Units::convertSpeed(_aircraftData.raw.airspeedMPS, SpeedUnit::meterPerSecond, SpeedUnit::knot),
			LPFFactor
		);
		
		// Altitude
		_aircraftData.computed.altitudeFt = applyLPF(
			_aircraftData.computed.altitudeFt,
			Units::convertDistance(_aircraftData.raw.coordinates.getAltitude(), DistanceUnit::meter, DistanceUnit::foot),
			LPFFactor
		);
		
		// Wind direction
		_aircraftData.computed.windDirectionRad = applyLPF(
			_aircraftData.computed.windDirectionRad,
			_aircraftData.raw.windDirectionRad,
			LPFFactor
		);
		
		// Throttle
		_aircraftData.computed.throttle_0_1 = applyLPF(
			_aircraftData.computed.throttle_0_1,
			static_cast<float>(_aircraftData.raw.throttle_0_255) / 255.f,
			LPFFactor
		);

		// Yaw trend
		_aircraftData.computed.yawTrendDeg = applyLPF(
			_aircraftData.computed.yawTrendDeg,
			_aircraftData.raw.yawTrendDeg,
			LPFFactor
		);

		// Slower
		LPFFactor = LowPassFilter::getDeltaTimeUsFactor(1.0f, deltaTimeUs);

		// Airspeed trend
		_aircraftData.computed.airspeedTrendKt = applyLPF(
			_aircraftData.computed.airspeedTrendKt,
			Units::convertSpeed(_aircraftData.raw.airspeedTrendMPS, SpeedUnit::meterPerSecond, SpeedUnit::knot),
			LPFFactor
		);
		
		// Altitude trend
		_aircraftData.computed.altitudeTrendFt = applyLPF(
			_aircraftData.computed.altitudeTrendFt,
			Units::convertDistance(_aircraftData.raw.altitudeTrendM, DistanceUnit::meter, DistanceUnit::foot),
			LPFFactor
		);
		
		// Vertical speed
		_aircraftData.computed.verticalSpeedFPM = applyLPF(
			_aircraftData.computed.verticalSpeedFPM,
			Units::convertDistance(_aircraftData.raw.verticalSpeedMPM, DistanceUnit::meter, DistanceUnit::foot),
			LPFFactor
		);
		
		// Smooth as fuck
		LPFFactor = LowPassFilter::getDeltaTimeUsFactor(0.5f, deltaTimeUs);
	}

	void Thermaller::batteryTick() {
		if (esp_timer_get_time() < _batteryTickTime)
			return;

		_battery.tick();

		_batteryTickTime = esp_timer_get_time() + 1'000'000 / (1 * 8);
	}

	// ------------------------- Data -------------------------

	Application& Thermaller::getApplication() {
		return _application;
	}
	
	RemoteData& Thermaller::getRemoteData() {
		return _remoteData;
	}
	
	AircraftData& Thermaller::getAircraftData() {
		return _aircraftData;
	}

	NavigationData& Thermaller::getNavigationData() {
		return _navigationData;
	}
	
	SemaphoreHandle_t Thermaller::getSPIMutex() const {
		return _SPIMutex;
	}

	void Thermaller::playFeedback(const Sound* sound) {
		if (_settings.personalization.audioFeedback) {
			_audioPlayer.play(sound);
		}
	}

	void Thermaller::playFeedback() {
		playFeedback(&resources::sounds::feedback);
	}

	Settings& Thermaller::getSettings() {
		return _settings;
	}
	
	AudioPlayer& Thermaller::getAudioPlayer() {
		return _audioPlayer;
	}

	PushButtonEncoder& Thermaller::getEncoder() {
		return _encoder;
	}

	Axes& Thermaller::getAxes() {
		return _axes;
	}

	Battery Thermaller::getBattery() const {
		return _battery;
	}

	void Thermaller::updateDebugOverlayVisibility() {
		if (_settings.personalization.debugOverlay) {
			if (_debugOverlay != nullptr)
				return;

			_debugOverlay = new DebugOverlay();
			_application += _debugOverlay;
		}
		else {
			if (_debugOverlay == nullptr)
				return;

			_application -= _debugOverlay;
			delete _debugOverlay;
			_debugOverlay = nullptr;
		}
	}

	const Route* Thermaller::getRoute() const {
		return _route;
	}

	void Thermaller::setRoute(const Route* route) {
		if (route == _route)
			return;

		_route = route;

		// Removing old page
		if (_pageLayout.getChildrenCount() > 0) {
			const auto oldPage = _pageLayout[0];

			_pageLayout.removeChildAt(0);

			delete oldPage;
		}

		// Adding new page
		if (_route)
			_pageLayout += _route->buildElement();
	}
	
	RemoteTransceiver& Thermaller::getTransceiver() {
		return _transceiver;
	}

	[[noreturn]] void Thermaller::startErrorLoop(const char* error) {
		ESP_LOGE(_logTag, "%s", error);
		
		while (true) {
			vTaskDelay(pdMS_TO_TICKS(1'000));
		}
	}
}