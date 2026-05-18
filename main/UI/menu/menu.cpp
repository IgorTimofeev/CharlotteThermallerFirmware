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
		_title.setMargin(Margin(20, 10, 0, 0));
		_title.setFont(&Theme::fontNormal);
		_title.setTextColor(&Theme::fg1);
		_titleAndItemLayout.setAutoSize(&_title);
		_titleAndItemLayout += &_title;

		// Title and item layout
		_titleAndItemLayout.setGap(2);
		*this += &_titleAndItemLayout;

		// Item scroll view
		_itemScrollView.setScrollBarThumbColor(&Theme::bg3);
		_itemScrollView.setHorizontalScrollMode(ScrollMode::disabled);
		_titleAndItemLayout += &_itemScrollView;

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
				_title.setText(L"Palette");
				setPage(&_palettePage);

				break;
			}
			case Route::emissivity: {
				_title.setText(L"Emissivity");
				setPage(&_emissivityPage);

				break;
			}
			default: break;
		}
	}

	void Menu::setPage(MenuPage* page) {
		_itemScrollView.removeChildren();
		_itemScrollView += page;
	}
}
