#include "UI/menu/mainMenuPage.h"

#include "thermaller.h"

namespace pizda {
	EmissivityMenuItem::EmissivityMenuItem() {
		setText(L"Emissivity");
		setMin(0);
		setMax(100);
		setStep(1);
		setValue(Thermaller::getInstance().settings.emissivityPercent);
	}

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

	void EmissivityMenuItem::onJoystickEvent(JoystickEvent* event) {
		IntMenuItem::onJoystickEvent(event);

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
			&interpolationItem,
			&exitItem
		});

		setSelectedIndex(0);
	}
}
