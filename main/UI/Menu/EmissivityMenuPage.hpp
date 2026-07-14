#pragma once

#include <YOBA/Core.hpp>
#include <YOBA/UI.hpp>

#include "UI/Menu/MenuPage.hpp"
#include "UI/Menu/MenuItem.hpp"
#include "UI/Menu/MenuRoute.hpp"

namespace pizda {
	class EmissivityMenuItem : public IntMenuItem {
		public:
			EmissivityMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class EmissivityPresetMenuItem : public MenuItem {
		public:
			EmissivityPresetMenuItem(const std::string_view title, const uint8_t emissivity);

			void onJoystickEvent(JoystickEvent* event) override;

		protected:
			void onRender(Renderer* renderer, const Rectangle& bounds) override;

		private:
			const uint8_t _emissivity;
	};

	class EmissivityMenuPage : public MenuPage {
		public:
			EmissivityMenuPage();

			EmissivityMenuItem manualItem {};
			EmissivityPresetMenuItem matteItem { "Matte", 95 };
			EmissivityPresetMenuItem semiGlossItem { "Semi-gloss", 80 };
			EmissivityPresetMenuItem glossyItem { "Semi-reflective", 45 };
			EmissivityPresetMenuItem reflectiveItem { "Reflective", 10 };
			RouteMenuItem backItem { "Back", MenuRoute::main };
	};
}