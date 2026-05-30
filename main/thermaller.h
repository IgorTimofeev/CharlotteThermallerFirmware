#pragma once

#include <esp_log.h>
#include <esp_adc/adc_oneshot.h>

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include <YOBA/hardware/displays/ST7789Display.h>

#include <audioPlayer.h>
#include <buzzer.h>
#include <ADCVoltmeter.h>

#include "config.h"
#include "UI/menu/menuRoute.h"
#include "UI/thermalView.h"
#include "UI/menu/menu.h"
#include "hardware/MLX90640/MLX90640.h"
#include "hardware/joystick/joystick.h"
#include "settings/settings.h"

namespace pizda {
	using namespace YOBA;

	class TestEblo : public Control, public FillColorElement, public TextElement {
		public:
			TestEblo(const Color* color, const std::wstring_view text) {
				setFillColor(color);
				setText(text);
				setSize(Size(80, 32));
				setHorizontalAlignment(Alignment::center);
			}

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override {
				renderer->renderFilledRectangle(bounds, getFillColor());

				// ESP_LOGI("pozuida", "bounds: %d, %d, %d, %d", bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());

				renderer->renderText(
					bounds.getCenter() - Vector2I(Theme::fontNormal.getWidth(getText()), Theme::fontNormal.getHeight()) / 2,
					&Theme::fontNormal,
					&Theme::bg1,
					getText()
				);
			}

		private:

	};

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

			RGB565PixelBufferRenderer renderer {};

			// Thermal sensor
			MLX90640 MLX {};

			// Battery
			TransistorControlledADCVoltmeter battery {
				config::battery::transistorPin,

				config::battery::ADCUnit,
				&ADCOneshotUnit1,
				config::battery::ADCChannel,

				config::battery::voltageMin,
				config::battery::voltageMax,
				
				config::battery::dividerResistanceR1,
				config::battery::dividerResistanceR2,

				8
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

			StackLayout rows {};

			TestEblo eblo1 { &Theme::red, L"Test 1"};

			ScaleTransform transform2 { 2, Vector2F( 0, 0 ) };
			TestEblo eblo2 { &Theme::green, L"Test 2"};

			ScaleTransform transform3 { 2, Vector2F( 0.5f, 0.5f) };
			TestEblo eblo3 { &Theme::orange, L"Test 3"};

			ScaleTransform transform4 { 2, Vector2F( 1, 1) };
			TestEblo eblo4 { &Theme::orange, L"Test 3"};

			void setRoute(const MenuRoute route);

			// -------------------------------- Other shit --------------------------------

			i2c_master_bus_handle_t I2CMasterBus {};
			adc_oneshot_unit_handle_t ADCOneshotUnit1 {};

			Settings settings;

		private:
			constexpr static auto _logTag = "Thermaller";

			Thermaller() = default;

			Menu* _menu = nullptr;
			MenuRoute _route = MenuRoute::none;

			int64_t _batteryTickTime = 0;
			void batteryTick();
	};
}