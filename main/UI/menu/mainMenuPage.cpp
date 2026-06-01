#include "UI/menu/mainMenuPage.h"

#include "thermaller.h"

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
