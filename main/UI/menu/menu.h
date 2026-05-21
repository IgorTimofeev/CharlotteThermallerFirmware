#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "menuRoute.h"
#include "UI/menu/menuPage.h"

namespace pizda {
	class Menu : public Layout {
		public:
			Menu();

			void setRoute(const MenuRoute route);

		private:
			Rectangle _backgroundRectangle {};

			MenuRoute _route = MenuRoute::none;
			MenuPage* _menuPage = nullptr;

			void setPage(MenuPage* page);
	};
}