#pragma once

#include <esp_log.h>
#include <esp_adc/adc_oneshot.h>

#include <YOBA/Core.hpp>
#include <YOBA/Rendering.hpp>
#include <YOBA/UI.hpp>
#include <YOBA/Hardware/Displays/ST7789Display.hpp>

#include <AudioPlayer.hpp>
#include <Buzzer.hpp>
#include <ADCVoltmeter.hpp>

#include "Config.hpp"
#include "UI/Menu/MenuRoute.hpp"
#include "UI/thermalView.hpp"
#include "UI/Menu/Menu.hpp"
#include "hardware/MLX90640/MLX90640.hpp"
#include "hardware/joystick/joystick.hpp"
#include "settings/settings.hpp"

namespace pizda {
	using namespace YOBA;

	class Thermaller {
		public:
			static Thermaller& getInstance();

			[[noreturn]] void start();

			// -------------------------------- Hardware --------------------------------

			ST7789Display display {};
			RGB565TransactionalBufferedRenderer renderer {};

			// Thermal sensor
			MLX90640 MLX {};

			// Battery
			TransistorControlledADCVoltmeter battery {};

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

			void setRoute(const MenuRoute route);

			// -------------------------------- Other shit --------------------------------

			i2c_master_bus_handle_t I2CMasterBus {};
			adc_oneshot_unit_handle_t ADCOneshotUnit1 {};

			Settings settings;

		private:
			constexpr static auto _logTag = "Thermaller";

			Thermaller() = default;

			Menu* _menu = nullptr;
			SizeAnimation _menuAnimation {};

			int64_t _batteryTickTime = 0;
			void batteryTick();
	};
}