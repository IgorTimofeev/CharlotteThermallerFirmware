#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "UI/route.h"

namespace pizda {
	class EmissivityMenuItem : public IntMenuItem {
		public:
			EmissivityMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class MainMenuPage : public MenuPage {
		public:
			MainMenuPage();

			RouteMenuItem paletteItem { L"Color palette", Route::palette };
			EmissivityMenuItem emissivityItem {};
			RouteMenuItem rangeItem { L"Temperature range", Route::range };
			RouteMenuItem exitItem { L"Exit", Route::none };
	};
}