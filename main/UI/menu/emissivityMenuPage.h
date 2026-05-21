#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "menuRoute.h"

namespace pizda {
	class EmissivityMenuItem : public IntMenuItem {
		public:
			EmissivityMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class EmissivityPresetMenuItem : public MenuItem {
		public:
			EmissivityPresetMenuItem(const std::wstring_view title, const uint8_t emissivity);

			void onJoystickEvent(JoystickEvent* event) override;

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;

		private:
			const uint8_t _emissivity;
	};

	class EmissivityMenuPage : public MenuPage {
		public:
			EmissivityMenuPage();

			EmissivityMenuItem manualItem {};
			EmissivityPresetMenuItem matteItem { L"Matte", 95 };
			EmissivityPresetMenuItem semiGlossItem { L"Semi-gloss", 80 };
			EmissivityPresetMenuItem glossyItem { L"Semi-reflective", 45 };
			EmissivityPresetMenuItem reflectiveItem { L"Reflective", 10 };
			RouteMenuItem backItem { L"Back", MenuRoute::main };
	};
}