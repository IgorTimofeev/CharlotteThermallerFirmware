#pragma once

#include <YOBA/Core.hpp>
#include <YOBA/UI.hpp>

#include "UI/Menu/MenuPage.hpp"
#include "UI/Menu/MenuItem.hpp"
#include "UI/Menu/MenuRoute.hpp"

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