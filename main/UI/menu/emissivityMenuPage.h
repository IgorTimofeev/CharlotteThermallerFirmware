#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/menu/menuPage.h"
#include "UI/menu/menuItem.h"
#include "UI/route.h"

namespace pizda {
	class EmissivityMenuPage : public MenuPage {
		public:
			EmissivityMenuPage();

			RouteMenuItem exitItem { L"Back", Route::main };
	};
}