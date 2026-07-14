#include <esp_adc/adc_oneshot.h>

#include "Hardware/Joystick/Joystick.hpp"
#include "Resources/Sounds.hpp"
#include "Thermaller.hpp"
#include "Config.hpp"

namespace pizda {
	JoystickEvent::JoystickEvent(const JoystickEventType type, const bool razyob) : Event(typeID), type(type), razyob(razyob) {

	}

	uint16_t JoystickEvent::typeID = registerTypeID();

	void Joystick::setup() {
		auto& th = Thermaller::getInstance();

		_axisX.setup(
			&th.ADCOneshotUnit1,
			config::joystick::axisX::channel,
			config::joystick::axisX::invertInput
		);

		_axisY.setup(
			&th.ADCOneshotUnit1,
			config::joystick::axisY::channel,
			config::joystick::axisY::invertInput
		);

		// Button
		{
			gpio_config_t g = {};
			g.pin_bit_mask = 1ULL << config::joystick::button;
			g.mode = GPIO_MODE_INPUT;
			g.pull_up_en = GPIO_PULLUP_ENABLE;
			g.pull_down_en = GPIO_PULLDOWN_DISABLE;
			g.intr_type = GPIO_INTR_LOW_LEVEL;
			gpio_config(&g);
		}
	}

	void Joystick::tick() {
		auto& th = Thermaller::getInstance();

		// Axes
		auto handleAxis = [&th](ButtonAxis& axis, const JoystickEventType positiveType, const JoystickEventType negativeType) {
			bool positivePressed = false;
			bool negativePressed = false;
			bool razyob = false;

			axis.tick();
			axis.check(positivePressed, negativePressed, razyob);

			if (positivePressed || negativePressed) {
				JoystickEvent event {
					positivePressed ? positiveType : negativeType,
					razyob
				};

				th.application.pushEvent(&event);
			}
		};

		handleAxis(_axisX, JoystickEventType::right, JoystickEventType::left);
		handleAxis(_axisY, JoystickEventType::up, JoystickEventType::down);

		// Button
		const auto buttonPressed = !gpio_get_level(config::joystick::button);

		if (buttonPressed && !_buttonPressed) {
			JoystickEvent event { JoystickEventType::press };
			th.application.pushEvent(&event);
		}

		_buttonPressed = buttonPressed;

		// const auto x = _axisX.getValue();
		// const auto y = _axisY.getValue();
		// const auto button = gpio_get_level( config::joystick::button);
		//
		// ESP_LOGI("Joystick", "x: %d, y: %d, button: %d", x, y, button);
	}
}
