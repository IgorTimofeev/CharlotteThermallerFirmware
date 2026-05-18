#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/route.h"
#include "UI/menu/menuPage.h"

#include "UI/menu/mainMenuPage.h"
#include "UI/menu/paletteMenuPage.h"
#include "UI/menu/rangeMenuPage.h"

namespace pizda {
	class Menu : public Layout {
		public:
			Menu();

			void setRoute(const Route route);

		private:
			Rectangle _backgroundRectangle {};
			RelativeStackLayout _titleAndItemLayout {};
			TextView _title {};

			MainMenuPage _mainPage {};
			PaletteMenuPage _palettePage {};
			RangeMenuPage _rangePage {};

			Route _route = Route::none;

			MenuPage* _menuPage = nullptr;

			void setPage(MenuPage* page);
	};
}