#include "UI/menu/rangeMenuPage.h"

#include <thermaller.h>

namespace pizda {
	AutoRangeMenuItem::AutoRangeMenuItem() {
		setText(L"Automatic");
		setValue(Thermaller::getInstance().settings.rangeAuto);
	}

	void AutoRangeMenuItem::onJoystickEvent(JoystickEvent* event) {
		BoolMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::press)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.rangeAuto = getValue();
		th.settings.scheduleWrite();
	}

	RangeMenuItem::RangeMenuItem() {
		setMin(-40);
		setMax(300);
		setSmallStep(1);
		setBigStep(10);
	}

	MinRangeMenuItem::MinRangeMenuItem() {
		setText(L"Minimum");
		setValue(Thermaller::getInstance().settings.rangeMin);
	}

	void MinRangeMenuItem::onJoystickEvent(JoystickEvent* event) {
		IntMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.rangeMin = std::min<int32_t>(getValue(), th.settings.rangeMax);
		th.settings.scheduleWrite();
	}

	MaxRangeMenuItem::MaxRangeMenuItem() {
		setText(L"Maximum");
		setValue(Thermaller::getInstance().settings.rangeMax);
	}

	void MaxRangeMenuItem::onJoystickEvent(JoystickEvent* event) {
		IntMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.rangeMax = std::max<int32_t>(getValue(), th.settings.rangeMin);
		th.settings.scheduleWrite();
	}

	RangeMenuPage::RangeMenuPage() : MenuPage(L"Temperature range") {
		addItems({
			&autoItem,
			&minItem,
			&maxItem,
			&backItem
		});

		setSelectedIndex(0);
	}
}
