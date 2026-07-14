#include "UI/Menu/Menu.hpp"

#include <span>
#include <functional>

#include <YOBA/Core.hpp>
#include <YOBA/UI.hpp>

#include "UI/Theme.hpp"
#include "UI/Menu/MenuRoute.hpp"
#include "UI/Menu/MenuItem.hpp"

#include "UI/Menu/MainMenuPage.hpp"
#include "UI/Menu/PaletteMenuPage.hpp"
#include "UI/Menu/HistogramMenuPage.hpp"
#include "UI/Menu/EmissivityMenuPage.hpp"
#include "UI/Menu/SensorMenuPage.hpp"
#include "UI/Menu/ReflectedTemperatureSensorMenuPage.hpp"

#include "Thermaller.hpp"

namespace pizda {
	Menu::Menu() {
		// setMaxHeight(215);
		setVerticalAlignment(Alignment::end);

		// Background
		_backgroundRectangle.setFillColor(&Theme::bg1);
		*this += &_backgroundRectangle;

		// Item layout
	}

	Menu::~Menu() {
		if (_menuPage)
			delete _menuPage;
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
			case MenuRoute::histogram: {
				setPage(new HistogramMenuPage());
				break;
			}
			case MenuRoute::sensor: {
				setPage(new SensorMenuPage());
				break;
			}
			case MenuRoute::sensorReflectedTemperature: {
				setPage(new ReflectedTemperatureSensorMenuPage());
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
