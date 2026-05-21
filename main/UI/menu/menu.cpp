#include "UI/menu/menu.h"

#include <span>
#include <functional>

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/theme.h"
#include "UI/menu/menuRoute.h"
#include "UI/menu/menuItem.h"

#include "UI/menu/mainMenuPage.h"
#include "UI/menu/paletteMenuPage.h"
#include "UI/menu/rangeMenuPage.h"
#include "UI/menu/emissivityMenuPage.h"
#include "UI/menu/sensorMenuPage.h"

#include "thermaller.h"

namespace pizda {
	Menu::Menu() {
		setMaxHeight(215);
		setVerticalAlignment(Alignment::end);

		// Background
		_backgroundRectangle.setFillColor(&Theme::bg1);
		*this += &_backgroundRectangle;

		// Item layout
	}

	void Menu::setRoute(const MenuRoute route) {
		_route = route;

		switch (_route) {
			case MenuRoute::main: {
				setPage(new MainMenuPage());
				break;
			}
			case MenuRoute::emissivity: {
				setPage(new EmissivityMenuPage());
				break;
			}
			case MenuRoute::palette: {
				setPage(new PaletteMenuPage());
				break;
			}
			case MenuRoute::range: {
				setPage(new RangeMenuPage());
				break;
			}
			case MenuRoute::sensor: {
				setPage(new SensorMenuPage());
				break;
			}
			default: break;
		}
	}

	void Menu::setPage(MenuPage* page) {
		if (_menuPage) {
			*this -= _menuPage;
			delete _menuPage;
		}

		_menuPage = page;
		*this += _menuPage;
	}
}
