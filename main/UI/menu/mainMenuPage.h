#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "UI/route.h"

namespace pizda {
	class MainMenuPage : public MenuPage {
		public:
			MainMenuPage();

			RouteMenuItem paletteItem { L"Color palette", Route::palette };
			RouteMenuItem emissivityItem { L"Emissivity", Route::emissivity };
			RouteMenuItem exitItem { L"Exit", Route::none };
	};
}