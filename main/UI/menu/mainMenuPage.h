#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "UI/route.h"

namespace pizda {
	class InterpolationMenuItem : public BoolMenuItem {
		public:
			InterpolationMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class MainMenuPage : public MenuPage {
		public:
			MainMenuPage();

			RouteMenuItem paletteItem { L"Color palette", Route::palette };
			RouteMenuItem emissivityItem { L"Emissivity", Route::emissivity};
			RouteMenuItem rangeItem { L"Temperature range", Route::range };
			InterpolationMenuItem interpolationItem {};
			RouteMenuItem exitItem { L"Exit", Route::none };
	};
}