#include "UI/menu/menu.h"

#include <span>
#include <functional>

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/route.h"
#include "UI/theme.h"

#include "UI/menu/mainMenuPage.h"
#include "UI/menu/paletteMenuPage.h"
#include "UI/menu/rangeMenuPage.h"

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
				setPage(L"Menu", new MainMenuPage());
				break;
			}
			case Route::palette: {
				setPage(L"Color palette", new PaletteMenuPage());
				break;
			}
			case Route::range: {
				setPage(L"Temperature range", new RangeMenuPage());
				break;
			}
			default: break;
		}
	}

	void Menu::setPage(const std::wstring_view title, MenuPage* page) {
		_title.setText(title);

		if (_menuPage) {
			_titleAndItemLayout -= _menuPage;
			delete _menuPage;
		}

		_menuPage = page;
		_titleAndItemLayout += _menuPage;
	}
}
