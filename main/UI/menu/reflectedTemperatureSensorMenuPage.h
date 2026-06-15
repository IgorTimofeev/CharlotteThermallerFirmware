#pragma once

#include <YOBA/core.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "menuRoute.h"

namespace pizda {
	class AutoReflectedTemperatureSensorMenuItem : public BoolMenuItem {
		public:
			AutoReflectedTemperatureSensorMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class ValueReflectedTemperatureSensorMenuItem : public IntMenuItem {
		public:
			ValueReflectedTemperatureSensorMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class ReflectedTemperatureSensorMenuPage : public MenuPage {
		public:
			ReflectedTemperatureSensorMenuPage();

			AutoReflectedTemperatureSensorMenuItem autoItem {};
			ValueReflectedTemperatureSensorMenuItem valueItem {};
			RouteMenuItem backItem { "Back", MenuRoute::sensor };
	};
}