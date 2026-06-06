#include "UI/menu/histogramMenuPage.h"

#include <thermaller.h>

namespace pizda {
	AutoHistogramMenuItem::AutoHistogramMenuItem() {
		setText("Automatic");
		setValue(Thermaller::getInstance().settings.rangeAuto);
	}

	void AutoHistogramMenuItem::onJoystickEvent(JoystickEvent* event) {
		BoolMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::press)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.rangeAuto = getValue();
		th.settings.writeLater();
	}

	HistogramMenuItem::HistogramMenuItem() {
		setMin(-40);
		setMax(300);
		setSmallStep(1);
		setBigStep(10);
	}

	MinHistogramMenuItem::MinHistogramMenuItem() {
		setText("Minimum");
		setValue(Thermaller::getInstance().settings.rangeMin);
	}

	void MinHistogramMenuItem::onJoystickEvent(JoystickEvent* event) {
		IntMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.rangeMin = std::min<int32_t>(getValue(), th.settings.rangeMax);
		th.settings.writeLater();
	}

	MaxHistogramMenuItem::MaxHistogramMenuItem() {
		setText("Maximum");
		setValue(Thermaller::getInstance().settings.rangeMax);
	}

	void MaxHistogramMenuItem::onJoystickEvent(JoystickEvent* event) {
		IntMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.rangeMax = std::max<int32_t>(getValue(), th.settings.rangeMin);
		th.settings.writeLater();
	}

	HistogramMenuPage::HistogramMenuPage() : MenuPage("Histogram") {
		addItems({
			&autoItem,
			&minItem,
			&maxItem,
			&backItem
		});

		setSelectedIndex(0);
	}
}
