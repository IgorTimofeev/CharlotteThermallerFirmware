#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "menuRoute.h"

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
			RouteMenuItem backItem { L"Back", MenuRoute::main };
	};
}