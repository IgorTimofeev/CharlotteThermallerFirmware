#pragma once

#include <YOBA/Core.hpp>
#include <YOBA/UI.hpp>

#include "UI/Menu/MenuPage.hpp"
#include "UI/Menu/MenuItem.hpp"
#include "UI/Menu/MenuRoute.hpp"

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