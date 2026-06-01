#include "UI/menu/sensorMenuPage.h"

#include "settings/settings.h"
#include "thermaller.h"

namespace pizda {
	InterpolationMenuItem::InterpolationMenuItem() {
		setText("Interpolation");
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

	RefreshRateMenuItem::RefreshRateMenuItem() {
		setText("Refresh rate");
		setVariantCount(static_cast<uint8_t>(SettingsRefreshRate::maxValue) + 1);
		setVariantIndex(static_cast<uint8_t>(Thermaller::getInstance().settings.refreshRate));
	}

	void RefreshRateMenuItem::onJoystickEvent(JoystickEvent* event) {
		VariantMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.refreshRate = static_cast<SettingsRefreshRate>(getVariantIndex());
		th.settings.scheduleWrite();

		th.MLX.setRefreshRateFromSettings();
	}

	const char* RefreshRateMenuItem::variantToString() {
		switch (static_cast<SettingsRefreshRate>(getVariantIndex())) {
			case SettingsRefreshRate::hz0_5: return "0.5 Hz";
			case SettingsRefreshRate::hz1: return "1 Hz";
			case SettingsRefreshRate::hz2: return "2 Hz";
			case SettingsRefreshRate::hz4: return "4 Hz";
			case SettingsRefreshRate::hz8: return "8 Hz";
			case SettingsRefreshRate::hz16: return "16 Hz";
			case SettingsRefreshRate::hz32: return "32 Hz";
			default: return "64 Hz";
		}
	}

	TemperatureShiftMenuItem::TemperatureShiftMenuItem() {
		setText("Temperature shift");
		setMin(-100);
		setMax(100);
		setSmallStep(1);
		setBigStep(5);
		setValue(Thermaller::getInstance().settings.temperatureShift);
	}

	void TemperatureShiftMenuItem::onJoystickEvent(JoystickEvent* event) {
		IntMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.temperatureShift = getValue();
		th.settings.scheduleWrite();
	}

	SensorMenuPage::SensorMenuPage() : MenuPage("Sensor") {
		addItems({
			&refreshRateItem,
			&temperatureShiftItem,
			&reflectedTemperatureItem,
			&interpolationItem,
			&backItem
		});

		setSelectedIndex(0);
	}
}
