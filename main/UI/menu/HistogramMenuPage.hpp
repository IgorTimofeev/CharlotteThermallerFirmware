#pragma once

#include <YOBA/Core.hpp>
#include <YOBA/UI.hpp>

#include "UI/Menu/MenuPage.hpp"
#include "UI/Menu/MenuItem.hpp"
#include "UI/Menu/MenuRoute.hpp"

namespace pizda {
	class AutoHistogramMenuItem : public BoolMenuItem {
		public:
			AutoHistogramMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class HistogramMenuItem : public IntMenuItem {
		public:
			HistogramMenuItem();
	};

	class MinHistogramMenuItem : public HistogramMenuItem {
		public:
			MinHistogramMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class MaxHistogramMenuItem : public HistogramMenuItem {
		public:
			MaxHistogramMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class HistogramMenuPage : public MenuPage {
		public:
			HistogramMenuPage();

			AutoHistogramMenuItem autoItem {};
			MinHistogramMenuItem minItem {};
			MaxHistogramMenuItem maxItem {};
			RouteMenuItem backItem { "Back", MenuRoute::main };
	};
}