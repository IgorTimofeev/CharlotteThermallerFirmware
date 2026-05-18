#pragma once

#include <esp_log.h>
#include <esp_adc/adc_oneshot.h>

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include <YOBA/hardware/displays/ST7789Display.h>

#include <audioPlayer.h>
#include <buzzer.h>
#include <battery.h>

#include "config.h"
#include "UI/route.h"
#include "UI/thermalView.h"
#include "UI/menu/menu.h"
#include "hardware/MLX90640/MLX90640.h"
#include "hardware/joystick/joystick.h"
#include "settings/settings.h"

namespace pizda {
	using namespace YOBA;

	class Thermaller {
		public:
			static Thermaller& getInstance();

			[[noreturn]] void start();

			// -------------------------------- Hardware --------------------------------

			ST7789Display display {
				config::SPI::MOSI,
				config::SPI::MISO,
				config::SPI::SCK,

				config::screen::SS,
				config::screen::DC,
				config::screen::RST,
				config::screen::SPIFrequency,

				Size(240, 320),
				ViewportRotation::clockwise0
			};

			RGB565PixelBufferRenderer renderer { };

			// Thermal sensor
			MLX90640 MLX {};

			// Battery
			Battery battery {
				config::battery::unit,
				&ADCOneshotUnit1,
				config::battery::channel,

				config::battery::voltageMin,
				config::battery::voltageMax,
				config::battery::voltageDividerR1,
				config::battery::voltageDividerR2
			};

			// Audio
			Buzzer buzzer {
				config::buzzer::gpio,
				config::buzzer::channel
			};

			AudioPlayer audioPlayer { &buzzer };

			// Joystick
			Joystick joystick {};

			// -------------------------------- UI --------------------------------

			Application application {};
			ThermalView thermalView {};

			void setRoute(const Route route);

			// -------------------------------- Other shit --------------------------------

			i2c_master_bus_handle_t I2CMasterBus {};
			adc_oneshot_unit_handle_t ADCOneshotUnit1 {};

			Settings settings;

		private:
			constexpr static auto _logTag = "Thermaller";

			Thermaller() = default;

			Menu* _menu = nullptr;
			Route _route = Route::none;
	};
}