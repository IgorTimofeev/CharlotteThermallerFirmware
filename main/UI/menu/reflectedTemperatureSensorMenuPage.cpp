#include "UI/menu/reflectedTemperatureSensorMenuPage.h"

#include <thermaller.h>

namespace pizda {
	AutoReflectedTemperatureSensorMenuItem::AutoReflectedTemperatureSensorMenuItem() {
		setText(L"Automatic");
		setValue(Thermaller::getInstance().settings.reflectedTemperatureAuto);
	}

	void AutoReflectedTemperatureSensorMenuItem::onJoystickEvent(JoystickEvent* event) {
		BoolMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::press)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.reflectedTemperatureAuto = getValue();
		th.settings.scheduleWrite();
	}

	ValueReflectedTemperatureSensorMenuItem::ValueReflectedTemperatureSensorMenuItem() {
		setMin(-40);
		setMax(300);
		setSmallStep(1);
		setBigStep(10);

		setText(L"Exact value");
		setValue(Thermaller::getInstance().settings.reflectedTemperatureValue);
	}

	void ValueReflectedTemperatureSensorMenuItem::onJoystickEvent(JoystickEvent* event) {
		IntMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.reflectedTemperatureValue = getValue();
		th.settings.scheduleWrite();
	}

	ReflectedTemperatureSensorMenuPage::ReflectedTemperatureSensorMenuPage() : MenuPage(L"Ambient temperature") {
		addItems({
			&autoItem,
			&valueItem,
			&backItem
		});

		setSelectedIndex(0);
	}
}
