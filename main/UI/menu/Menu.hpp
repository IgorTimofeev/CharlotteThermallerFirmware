#pragma once

#include <YOBA/Core.hpp>
#include <YOBA/UI.hpp>

#include "UI/Menu/MenuRoute.hpp"
#include "UI/Menu/MenuPage.hpp"

namespace pizda {
	class Menu : public Layout {
		public:
			Menu();
			~Menu() override;

			void setRoute(const MenuRoute route);

		private:
			RectangularShape _backgroundRectangle {};

			MenuRoute _route = MenuRoute::none;
			MenuPage* _menuPage = nullptr;

			void setPage(MenuPage* page);
	};
}