#include <cstdint>
#include <esp_timer.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_log.h>

#include <driver/i2c_master.h>

#include "Thermaller.hpp"

#include <nvs_flash.h>

#include "Config.hpp"
#include "UI/Theme.hpp"
#include "Resources/Images.hpp"
#include "Resources/Sounds.hpp"

namespace pizda {
	using namespace YOBA;

	Thermaller& Thermaller::getInstance() {
		static auto instance = Thermaller();

		return instance;
	}

	[[noreturn]] void Thermaller::start() {
		// -------------------------------- Splash screen --------------------------------

		// First, let's render a splash screen while we wait for the peripherals to finish warming up

		// I2C
		{
			i2c_master_bus_config_t bus {};
			bus.clk_source = I2C_CLK_SRC_DEFAULT;
			bus.i2c_port = config::I2C::port;
			bus.scl_io_num = config::I2C::SCL;
			bus.sda_io_num = config::I2C::SDA;
			bus.glitch_ignore_cnt = 7;
			bus.flags.enable_internal_pullup = true;

			const auto state = i2c_new_master_bus(&bus, &I2CMasterBus);
			ESP_ERROR_CHECK_WITHOUT_ABORT(state);
		}

		// Display
		displayInterface.setup(
			config::SPI::MOSI,
			config::SPI::SCK,
			config::Screen::SS,
			config::Screen::DC,
			config::Screen::RST,

			SPI2_HOST,
			0,
			config::Screen::SPIFrequency
		);

		display.setup(
			&displayInterface,

			Size(240, 320),
			Rotation::none,
			ColorModel::RGB565
		);

		renderer.setup();
		renderer.setTarget(&display);

		// Rendering splash screen
		renderer.clear(&Theme::bg1);
		renderer.putImage(Rectangle(display.getSize()), &Images::splashScreen);
		renderer.flush();

		// Turning display on
		display.turnOn();

		// -------------------------------- Hardware --------------------------------

		// Settings come first
		settings.read();

		// ADC
		{
			adc_oneshot_unit_init_cfg_t unitConfig {};
			unitConfig.unit_id = ADC_UNIT_1;
			unitConfig.clk_src = ADC_RTC_CLK_SRC_DEFAULT;
			unitConfig.ulp_mode = ADC_ULP_MODE_DISABLE;
			ESP_ERROR_CHECK(adc_oneshot_new_unit(&unitConfig, &ADCOneshotUnit1));
		}

		// Battery
		batteryVoltmeter.setup(
			config::Battery::transistorPin,

			config::Battery::ADCUnit,
			ADCOneshotUnit1,
			config::Battery::ADCChannel,

			config::Battery::dividerResistanceR1,
			config::Battery::dividerResistanceR2
		);

		battery.setup(
			&batteryVoltmeter,
			config::Battery::voltageMin,
			config::Battery::voltageMax,
			8
		);

		// Audio
		audioPlayer.setup();

		// Joystick
		joystick.setup();

		// Thermal sensor
		MLX.setup(&I2CMasterBus);

		xTaskCreatePinnedToCore(
			[](void* arg) {
				while (true) {
					static_cast<MLX90640*>(arg)->tick();
				}
			},
			"MLX",
			4 * 1024,
			&MLX,
			20,
			nullptr,
			1
		);

		// -------------------------------- UI --------------------------------

		application.setRenderer(&renderer);
		application.setBackgroundColor(&Theme::bg1);

		application += &thermalView;

		// -------------------------------- Main loop --------------------------------

		audioPlayer.play(&resources::sounds::boot);

		// This shit is blazingly 🔥 fast 🚀, so letting user enjoy logo for a few moments
		vTaskDelay(pdMS_TO_TICKS(500));

		while (true) {
			joystick.tick();
			batteryTick();

			application.tick();
			application.updateLayout();
			application.render();

			vTaskDelay(settings.interpolation ? 1 : pdMS_TO_TICKS(1'000 / 32));
		}
	}

	void Thermaller::setRoute(const MenuRoute route) {
		if (route == MenuRoute::none) {
			if (_menu) {
				// Animation
				_menuAnimation.setTarget(_menu);
				_menuAnimation.stop();

				_menuAnimation.setFrom({ application.getSize().getWidth(), Size::computed });
				_menuAnimation.setTo({ application.getSize().getWidth(), 0 });
				_menuAnimation.setDuration(150'000);

				_menuAnimation.setOnStateChanged([this](const AnimationState state) {
					if (state != AnimationState::completed)
						return;

					application -= _menu;
					delete _menu;
					_menu = nullptr;
				});

				_menuAnimation.start();
			}
		}
		else {
			const auto menuDoesntExist = _menu == nullptr;

			if (menuDoesntExist) {
				_menu = new Menu();
				application += _menu;
			}

			_menu->setRoute(route);

			// Animation
			_menuAnimation.setTarget(_menu);
			_menuAnimation.stop();

			_menuAnimation.setFrom({ application.getSize().getWidth(), menuDoesntExist ? static_cast<uint16_t>(0) : Size::computed });
			_menuAnimation.setTo({ application.getSize().getWidth(), Size::computed });
			_menuAnimation.setDuration(150'000);
			_menuAnimation.setOnStateChanged(nullptr);

			_menuAnimation.start();
		}
	}

	void Thermaller::batteryTick() {
		if (esp_timer_get_time() < _batteryTickTime)
			return;

		battery.tick();
		// ESP_LOGI("m", "volt: %d, charge: %f", battery.getVoltageMV(), battery.getChargeFloat());

		_batteryTickTime = esp_timer_get_time() + 1'000'000 / battery.getMultisamplingThreshold();
	}
}
