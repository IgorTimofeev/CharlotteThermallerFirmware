#include "UI/Menu/ReflectedTemperatureSensorMenuPage.hpp"

#include "Thermaller.hpp"

namespace pizda {
	AutoReflectedTemperatureSensorMenuItem::AutoReflectedTemperatureSensorMenuItem() {
		setText("Automatic");
		setValue(Thermaller::getInstance().settings.reflectedTemperatureAuto);
	}

	void AutoReflectedTemperatureSensorMenuItem::onJoystickEvent(JoystickEvent* event) {
		BoolMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::press)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.reflectedTemperatureAuto = getValue();
		th.settings.writeLater();
	}

	ValueReflectedTemperatureSensorMenuItem::ValueReflectedTemperatureSensorMenuItem() {
		setMin(-40);
		setMax(300);
		setSmallStep(1);
		setBigStep(10);

		setText("Exact value");
		setValue(Thermaller::getInstance().settings.reflectedTemperatureValue);
	}

	void ValueReflectedTemperatureSensorMenuItem::onJoystickEvent(JoystickEvent* event) {
		IntMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.reflectedTemperatureValue = getValue();
		th.settings.writeLater();
	}

	ReflectedTemperatureSensorMenuPage::ReflectedTemperatureSensorMenuPage() : MenuPage("Ambient temperature") {
		addItems({
			&autoItem,
			&valueItem,
			&backItem
		});

		setSelectedIndex(0);
	}
}
