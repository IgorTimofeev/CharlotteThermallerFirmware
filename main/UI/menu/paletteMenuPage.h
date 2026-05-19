#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "UI/route.h"

namespace pizda {
	class PaletteMenuItem : public MenuItem {
		public:
			PaletteMenuItem(const std::wstring_view text, const ThermalPalette palette);

			ThermalPalette getRoute() const;

			void onJoystickEvent(JoystickEvent* event) override;

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;

		private:
			const ThermalPalette _palette;
	};

	class PaletteMenuPage : public MenuPage {
		public:
			PaletteMenuPage();

			PaletteMenuItem huntingItem { L"Hunting", ThermalPalette::hunting };
			PaletteMenuItem ironbowItem { L"Ironbow", ThermalPalette::ironbow };
			PaletteMenuItem rainbowItem { L"Rainbow", ThermalPalette::rainbow };
			PaletteMenuItem arcticItem { L"Arctic", ThermalPalette::arctic };
			PaletteMenuItem blackHotItem { L"Black hot", ThermalPalette::whiteHot };
			PaletteMenuItem whiteHotItem { L"White hot", ThermalPalette::blackHot };
			RouteMenuItem backItem { L"Back", Route::main };
	};}