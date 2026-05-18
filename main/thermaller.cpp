#include <cstdint>
#include <esp_timer.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_log.h>

#include <driver/i2c_master.h>

#include "thermaller.h"

#include <nvs_flash.h>

#include "config.h"
#include "UI/theme.h"
#include "resources/images.h"
#include "resources/sounds.h"

namespace pizda {
	using namespace YOBA;

	Thermaller& Thermaller::getInstance() {
		static auto instance = Thermaller();

		return instance;
	}

	[[noreturn]] void Thermaller::start() {
		ESP_LOGI(_logTag, "1");

		// -------------------------------- Splash screen --------------------------------

		// First, let's render a splash screen while we wait for the peripherals to finish warming up

		// // Multicore
		// {
		// 	_SPIMutex = xSemaphoreCreateMutex();
		// 	system::SPI::setMutex(_SPIMutex);
		// }

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

		ESP_LOGI(_logTag, "2");

		// SPI
		{
			spi_bus_config_t config {};
			config.mosi_io_num = config::SPI::MOSI;
			config.miso_io_num = config::SPI::MISO;
			config.sclk_io_num = config::SPI::SCK;
			config.quadwp_io_num = -1;
			config.quadhd_io_num = -1;
			config.max_transfer_sz = static_cast<int>(display.getSize().getSquare() * 2);

			ESP_ERROR_CHECK(spi_bus_initialize(config::SPI::hostDevice, &config, SPI_DMA_CH_AUTO));
		}

		ESP_LOGI(_logTag, "3");

		// Display
		display.setup();
		renderer.setTarget(&display);

		ESP_LOGI(_logTag, "4");

		// Rendering splash screen
		renderer.clear(&Theme::bg1);
		renderer.renderImage(Point(), &resources::images::splashScreen);
		renderer.flush();

		ESP_LOGI(_logTag, "5");

		// Turning display on
		display.turnOn();

		ESP_LOGI(_logTag, "6");

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

		// Settings come first because they contain XCVR modulation params, ADC axes calibration data, etc.
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
		battery.setup();

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
			application.tick();
			application.render();
			joystick.tick();

			vTaskDelay(pdMS_TO_TICKS(1'000 / 60));
		}
	}

	void Thermaller::setRoute(const Route route) {
		_route = route;

		if (_route == Route::none) {
			if (_menuAdded) {
				application -= &menu;
				_menuAdded = false;
			}
		}
		else {
			if (!_menuAdded) {
				application += &menu;
				_menuAdded = true;
			}

			menu.setRoute(_route);
		}
	}
}
