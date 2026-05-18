#include "joystick.h"

#include <esp_adc/adc_oneshot.h>
#include <resources/sounds.h>

#include "thermaller.h"
#include "config.h"

namespace pizda {
	JoystickEvent::JoystickEvent(const JoystickEventType type, const bool multi) : Event(typeID), type(type), multi(multi) {

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
		auto handleAxis = [&th](ButtonAxis& axis, JoystickEventType positiveType, JoystickEventType negativeType) {
			bool positivePressed = false;
			bool negativePressed = false;

			axis.tick();
			axis.check(positivePressed, negativePressed);

			if (positivePressed || negativePressed) {
				JoystickEvent event { positivePressed ? positiveType : negativeType };
				th.application.pushEvent(&event);

				th.audioPlayer.play(&resources::sounds::feedback);
			}
		};

		handleAxis(_axisX, JoystickEventType::right, JoystickEventType::left);
		handleAxis(_axisY, JoystickEventType::up, JoystickEventType::down);

		// Button
		const auto buttonPressed = !gpio_get_level(config::joystick::button);

		if (buttonPressed && !_buttonPressed) {
			JoystickEvent event { JoystickEventType::press };
			th.application.pushEvent(&event);

			th.audioPlayer.play(&resources::sounds::feedback);
		}

		_buttonPressed = buttonPressed;

		// const auto x = _axisX.getValue();
		// const auto y = _axisY.getValue();
		// const auto button = gpio_get_level( config::joystick::button);
		//
		// ESP_LOGI("Joystick", "x: %d, y: %d, button: %d", x, y, button);
	}
}
