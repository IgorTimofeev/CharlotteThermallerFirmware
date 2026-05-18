#include "UI/menu/mainMenuPage.h"

#include "thermaller.h"

namespace pizda {
	EmissivityMenuItem::EmissivityMenuItem() {
		setText(L"Emissivity");
		setMin(0);
		setMax(100);
		setStep(10);
		setValue(Thermaller::getInstance().settings.emissivityPercent);
	}

	void EmissivityMenuItem::onJoystickEvent(JoystickEvent* event) {
		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.emissivityPercent = getValue();
		th.settings.scheduleWrite();
	}

	MainMenuPage::MainMenuPage() {
		addItems({
			&paletteItem,
			&emissivityItem,
			&rangeItem,
			&exitItem
		});

		setSelectedIndex(0);
	}
}
