#pragma once

#include <YOBA/Core.hpp>

#include "Hardware/Joystick/Axis.hpp"

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
			JoystickEvent(const JoystickEventType type, const bool razyob = false);

			static uint16_t typeID;

			const JoystickEventType type;
			const bool razyob;
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