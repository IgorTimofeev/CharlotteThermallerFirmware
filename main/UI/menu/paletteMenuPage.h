#pragma once

#include <YOBA/core.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "menuRoute.h"

namespace pizda {
	class PaletteMenuItem : public MenuItem {
		public:
			PaletteMenuItem(const std::string_view text, const ThermalPalette palette);

			ThermalPalette getRoute() const;

			void onJoystickEvent(JoystickEvent* event) override;

		protected:
			void onRender(Renderer* renderer, const Rectangle& bounds) override;

		private:
			const ThermalPalette _palette;
	};

	class PaletteMenuPage : public MenuPage {
		public:
			PaletteMenuPage();

			PaletteMenuItem huntingItem { "Hunting", ThermalPalette::hunting };
			PaletteMenuItem ironbowItem { "Ironbow", ThermalPalette::ironbow };
			PaletteMenuItem arcticItem { "Arctic", ThermalPalette::arctic };
			PaletteMenuItem blackHotItem { "Black hot", ThermalPalette::whiteHot };
			PaletteMenuItem whiteHotItem { "White hot", ThermalPalette::blackHot };
			RouteMenuItem backItem { "Back", MenuRoute::main };
	};}