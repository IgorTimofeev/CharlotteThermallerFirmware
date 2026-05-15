#pragma once

#include <esp_log.h>
#include <esp_adc/adc_oneshot.h>

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include <YOBA/hardware/displays/ST7789Display.h>
#include <YOBA/hardware/encoder.h>

#include "config.h"
#include "UI/thermalView.h"
#include "hardware/MLX90640.h"

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

			//
			// // Battery
			// Battery _battery {
			// 	config::battery::remote::unit,
			// 	getAssignedADCOneshotUnit(config::battery::remote::unit),
			// 	config::battery::remote::channel,
			//
			// 	config::battery::remote::voltageMin,
			// 	config::battery::remote::voltageMax,
			// 	config::battery::remote::voltageDividerR1,
			// 	config::battery::remote::voltageDividerR2
			// };
			//
			// // Audio
			// Buzzer _buzzer {
			// 	config::buzzer::gpio,
			// 	config::buzzer::channel
			// };
			//
			// AudioPlayer _audioPlayer { &_buzzer };

			// -------------------------------- UI --------------------------------

			Application application {};
			ThermalView thermalView {};

			// const Route* _route = nullptr;

			// -------------------------------- Other shit --------------------------------

			i2c_master_bus_handle_t I2CMasterBusHandle {};

			// Settings _settings;

		private:
			constexpr static auto _logTag = "Thermaller";
			
			Thermaller() = default;
	};
}