#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "menuRoute.h"

namespace pizda {
	class InterpolationMenuItem : public BoolMenuItem {
		public:
			InterpolationMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class MainMenuPage : public MenuPage {
		public:
			MainMenuPage();

			RouteMenuItem paletteItem { L"Color palette", MenuRoute::palette };
			RouteMenuItem emissivityItem { L"Emissivity", MenuRoute::emissivity};
			RouteMenuItem rangeItem { L"Temperature range", MenuRoute::range };
			InterpolationMenuItem interpolationItem {};
			RouteMenuItem exitItem { L"Close", MenuRoute::none };
	};
}