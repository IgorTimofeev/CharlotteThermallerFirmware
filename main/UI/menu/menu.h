#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/route.h"
#include "UI/menu/menuPage.h"

namespace pizda {
	class Menu : public Layout {
		public:
			Menu();

			void setRoute(const Route route);

		private:
			Rectangle _backgroundRectangle {};
			RelativeStackLayout _titleAndItemLayout {};
			TextView _title {};

			Route _route = Route::none;
			MenuPage* _menuPage = nullptr;

			void setPage(std::wstring_view title, MenuPage* page);
	};
}