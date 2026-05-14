#pragma once

#include <esp_log.h>
#include <esp_adc/adc_oneshot.h>

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include <YOBA/hardware/displays/ILI9341Display.h>
#include <YOBA/hardware/touchPanels/FT6336UTouchPanel.h>
#include <YOBA/hardware/encoder.h>

#include "config.h"

namespace pizda {
	using namespace YOBA;

	class Thermaller {
		public:
			static Thermaller& getInstance();

			[[noreturn]] void start();

		private:
			constexpr static auto _logTag = "Thermaller";
			
			Thermaller() = default;
			// -------------------------------- Hardware --------------------------------

			// ILI9341Display _display {
			// 	config::SPI::MOSI,
			// 	config::SPI::MISO,
			// 	config::SPI::SCK,
			//
			// 	config::screen::SS,
			// 	config::screen::DC,
			// 	config::screen::RST,
			// 	config::screen::frequency
			// };
			//
			// Bit8PaletteRenderer _renderer { 32 };
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
			//
			// // -------------------------------- UI --------------------------------
			//
			// Application _application {};
			//
			// const Route* _route = nullptr;
			//
			// // -------------------------------- Other shit --------------------------------
			//
			// Settings _settings;
	};
}