#include "UI/Menu/SensorMenuPage.hpp"

#include "Settings/Settings.hpp"
#include "Thermaller.hpp"

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
		th.settings.writeLater();
	}

	SampleRateMenuItem::SampleRateMenuItem() {
		setText("Sample rate");
		setVariantCount(static_cast<uint8_t>(SettingsSampleRate::maxValue) + 1);
		setVariantIndex(static_cast<uint8_t>(Thermaller::getInstance().settings.sampleRate));
	}

	void SampleRateMenuItem::onJoystickEvent(JoystickEvent* event) {
		VariantMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.sampleRate = static_cast<SettingsSampleRate>(getVariantIndex());
		th.settings.writeLater();

		th.MLX.setRefreshRateFromSettings();
	}

	const char* SampleRateMenuItem::variantToString() {
		switch (static_cast<SettingsSampleRate>(getVariantIndex())) {
			case SettingsSampleRate::hz0_5: return "0.5 Hz";
			case SettingsSampleRate::hz1: return "1 Hz";
			case SettingsSampleRate::hz2: return "2 Hz";
			case SettingsSampleRate::hz4: return "4 Hz";
			case SettingsSampleRate::hz8: return "8 Hz";
			case SettingsSampleRate::hz16: return "16 Hz";
			case SettingsSampleRate::hz32: return "32 Hz";
			default: return "64 Hz";
		}
	}

	SensorMenuPage::SensorMenuPage() : MenuPage("Sensor") {
		addItems({
			&sampleRateItem,
			&reflectedTemperatureItem,
			&interpolationItem,
			&backItem
		});

		setSelectedIndex(0);
	}
}
