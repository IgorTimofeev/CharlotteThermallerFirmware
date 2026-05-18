#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "UI/route.h"

namespace pizda {
	class AutoRangeMenuItem : public BoolMenuItem {
		public:
			AutoRangeMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class RangeMenuItem : public IntMenuItem {
		public:
			RangeMenuItem();
	};

	class MinRangeMenuItem : public RangeMenuItem {
		public:
			MinRangeMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class MaxRangeMenuItem : public RangeMenuItem {
		public:
			MaxRangeMenuItem();

			void onJoystickEvent(JoystickEvent* event) override;
	};

	class RangeMenuPage : public MenuPage {
		public:
			RangeMenuPage();

			AutoRangeMenuItem autoItem {};
			MinRangeMenuItem minItem {};
			MaxRangeMenuItem maxItem {};
			RouteMenuItem backItem { L"Back", Route::main };
	};
}