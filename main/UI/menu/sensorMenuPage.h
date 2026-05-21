#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "menuRoute.h"

namespace pizda {
	class RefreshRateMenuItem : public VariantMenuItem {
		public:
			RefreshRateMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;

		protected:
			const wchar_t* variantToString() override;
	};

	class TemperatureShiftMenuItem : public IntMenuItem {
		public:
			TemperatureShiftMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class ChessModeMenuItem : public BoolMenuItem {
		public:
			ChessModeMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class InterpolationMenuItem : public BoolMenuItem {
		public:
			InterpolationMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class SensorMenuPage : public MenuPage {
		public:
			SensorMenuPage();

			RefreshRateMenuItem refreshRateItem {};
			RouteMenuItem reflectedTemperatureItem { L"Ambient temperature", MenuRoute::sensorReflectedTemperature };
			TemperatureShiftMenuItem temperatureShiftItem {};
			ChessModeMenuItem chessModeItem {};
			InterpolationMenuItem interpolationItem {};
			RouteMenuItem backItem { L"Back", MenuRoute::main };
	};
}