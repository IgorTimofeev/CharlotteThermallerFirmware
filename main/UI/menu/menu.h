#pragma once

#include <YOBA/core.h>
#include <YOBA/UI.h>

#include "menuRoute.h"
#include "UI/menu/menuPage.h"

namespace pizda {
	class Menu : public Layout {
		public:
			Menu();
			~Menu() override;

			void setRoute(const MenuRoute route);

		private:
			RectangleShape _backgroundRectangle {};

			MenuRoute _route = MenuRoute::none;
			MenuPage* _menuPage = nullptr;

			void setPage(MenuPage* page);
	};
}