#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "menuRoute.h"

namespace pizda {
	class MainMenuPage : public MenuPage {
		public:
			MainMenuPage();

			RouteMenuItem paletteItem { L"Palette", MenuRoute::palette };
			RouteMenuItem emissivityItem { L"Emissivity", MenuRoute::emissivity};
			RouteMenuItem rangeItem { L"Histogram", MenuRoute::histogram };
			RouteMenuItem sensorItem { L"Sensor", MenuRoute::sensor };
			RouteMenuItem exitItem { L"Close", MenuRoute::none };
	};
}