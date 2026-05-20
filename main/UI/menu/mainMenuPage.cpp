#include "UI/menu/mainMenuPage.h"

#include "thermaller.h"

namespace pizda {
	InterpolationMenuItem::InterpolationMenuItem() {
		setText(L"Interpolation");
		setValue(Thermaller::getInstance().settings.interpolation);
	}

	void InterpolationMenuItem::onJoystickEvent(JoystickEvent* event) {
		BoolMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::press)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.interpolation = getValue();
		th.settings.scheduleWrite();
	}

	MainMenuPage::MainMenuPage() {
		addItems({
			&paletteItem,
			&emissivityItem,
			&rangeItem,
			&interpolationItem,
			&exitItem
		});

		setSelectedIndex(0);
	}
}
