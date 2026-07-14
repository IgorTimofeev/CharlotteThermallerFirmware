#include "UI/Menu/MainMenuPage.hpp"

#include "Thermaller.hpp"

namespace pizda {
	MainMenuPage::MainMenuPage() : MenuPage("Settings") {
		addItems({
			&paletteItem,
			&emissivityItem,
			&rangeItem,
			&sensorItem,
			&exitItem
		});

		setSelectedIndex(0);
	}
}
