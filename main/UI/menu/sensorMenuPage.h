#pragma once

#include <YOBA/core.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "menuRoute.h"

namespace pizda {
	class SampleRateMenuItem : public VariantMenuItem {
		public:
			SampleRateMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;

		protected:
			const char* variantToString() override;
	};


	class InterpolationMenuItem : public BoolMenuItem {
		public:
			InterpolationMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class SensorMenuPage : public MenuPage {
		public:
			SensorMenuPage();

			SampleRateMenuItem sampleRateItem {};
			RouteMenuItem reflectedTemperatureItem { "Ambient temperature", MenuRoute::sensorReflectedTemperature };
			InterpolationMenuItem interpolationItem {};
			RouteMenuItem backItem { "Back", MenuRoute::main };
	};
}