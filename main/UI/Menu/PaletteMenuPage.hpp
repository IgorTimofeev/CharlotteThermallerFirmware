#pragma once

#include <YOBA/Core.hpp>
#include <YOBA/UI.hpp>

#include "UI/Menu/MenuPage.hpp"
#include "UI/Menu/MenuItem.hpp"
#include "UI/Menu/MenuRoute.hpp"

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