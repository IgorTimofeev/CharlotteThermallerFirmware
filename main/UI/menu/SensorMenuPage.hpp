#pragma once

#include <YOBA/Core.hpp>
#include <YOBA/UI.hpp>

#include "UI/Menu/MenuPage.hpp"
#include "UI/Menu/MenuItem.hpp"
#include "UI/Menu/MenuRoute.hpp"

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