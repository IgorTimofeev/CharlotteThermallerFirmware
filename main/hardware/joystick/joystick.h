#pragma once

#include <YOBA/main.h>

#include "hardware/joystick/axis.h"

namespace pizda {
	using namespace YOBA;

	enum class JoystickEventType : uint8_t {
		up,
		down,
		left,
		right,
		press
	};

	class JoystickEvent : public Event {
		public:
			JoystickEvent(const JoystickEventType type);

			static uint16_t typeID;

			const JoystickEventType type;
	};

	class Joystick {
		public:
			void setup();
			void tick();

		private:
			ButtonAxis _axisX {
				0,
				4095
			};

			ButtonAxis _axisY {
				0,
				4095
			};

			bool _buttonPressed = false;
	};
}