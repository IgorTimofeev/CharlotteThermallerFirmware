#include "UI/menu/emissivityMenuPage.h"

namespace pizda {
	EmissivityMenuPage::EmissivityMenuPage() {
		addItems({
			&exitItem
		});

		setSelectedIndex(0);
	}
}
