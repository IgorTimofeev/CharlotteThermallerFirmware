#include "UI/menu/mainMenuPage.h"

namespace pizda {
	MainMenuPage::MainMenuPage() {
		addItems({
			&paletteItem,
			&emissivityItem,
			&exitItem
		});

		setSelectedIndex(0);
	}
}
