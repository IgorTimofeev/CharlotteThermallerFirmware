#pragma once

#include <YOBA/core.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "menuRoute.h"

namespace pizda {
	class MainMenuPage : public MenuPage {
		public:
			MainMenuPage();

			RouteMenuItem paletteItem { "Palette", MenuRoute::palette };
			RouteMenuItem emissivityItem { "Emissivity", MenuRoute::emissivity};
			RouteMenuItem rangeItem { "Histogram", MenuRoute::histogram };
			RouteMenuItem sensorItem { "Sensor", MenuRoute::sensor };
			RouteMenuItem exitItem { "Close", MenuRoute::none };
	};
}