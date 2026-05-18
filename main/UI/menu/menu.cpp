#include "UI/menu/menu.h"

#include <span>
#include <functional>

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/route.h"
#include "UI/theme.h"

#include "thermaller.h"

namespace pizda {
	Menu::Menu() {
		setHeight(200);
		setVerticalAlignment(Alignment::end);

		// Background
		_backgroundRectangle.setFillColor(&Theme::bg1);
		*this += &_backgroundRectangle;

		// Title
		_title.setMargin(Margin(20, 20, 0, 10));
		_title.setFont(&Theme::fontNormal);
		_title.setTextColor(&Theme::fg1);
		_titleAndItemLayout.setAutoSize(&_title);
		_titleAndItemLayout += &_title;

		// Title and item layout
		*this += &_titleAndItemLayout;

		// Item layout
	}

	void Menu::setRoute(const Route route) {
		_route = route;

		switch (_route) {
			case Route::main: {
				_title.setText(L"Menu");
				setPage(&_mainPage);

				break;
			}
			case Route::palette: {
				_title.setText(L"Color palette");
				setPage(&_palettePage);

				break;
			}
			case Route::range: {
				_title.setText(L"Temperature range");
				setPage(&_rangePage);

				break;
			}
			default: break;
		}
	}

	void Menu::setPage(MenuPage* page) {
		if (_menuPage)
			_titleAndItemLayout -= _menuPage;

		_menuPage = page;
		_titleAndItemLayout += _menuPage;
	}
}
