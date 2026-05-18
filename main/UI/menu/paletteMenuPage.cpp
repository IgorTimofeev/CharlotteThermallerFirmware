#include "UI/menu/paletteMenuPage.h"

#include "thermaller.h"

namespace pizda {
	PaletteMenuItem::PaletteMenuItem(const std::wstring_view text, const ThermalPalette palette): MenuItem(text), _palette(palette) {

	}

	ThermalPalette PaletteMenuItem::getRoute() const {
		return _palette;
	}

	void PaletteMenuItem::onJoystickEvent(JoystickEvent* event) {
		if (event->type != JoystickEventType::press)
			return;

		auto& th = Thermaller::getInstance();

		th.settings.thermalPalette = _palette;
		th.settings.scheduleWrite();

		invalidate();
	}

	PaletteMenuPage::PaletteMenuPage() {
		addItems({
			&govnoItem,
			&ironbowItem,
			&whiteHotItem
		});

		setSelectedIndex(0);
	}
}
