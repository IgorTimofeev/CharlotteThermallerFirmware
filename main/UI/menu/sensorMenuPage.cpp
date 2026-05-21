#include "UI/menu/sensorMenuPage.h"

#include "settings/settings.h"
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

	RefreshRateMenuItem::RefreshRateMenuItem() {
		setText(L"Refresh rate");
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

	const wchar_t* RefreshRateMenuItem::variantToString() {
		switch (static_cast<SettingsRefreshRate>(getVariantIndex())) {
			case SettingsRefreshRate::hz1: return L"1 Hz";
			case SettingsRefreshRate::hz2: return L"2 Hz";
			case SettingsRefreshRate::hz4: return L"4 Hz";
			case SettingsRefreshRate::hz8: return L"8 Hz";
			case SettingsRefreshRate::hz16: return L"16 Hz";
			default: return L"32 Hz";
		}
	}

	AmbientTemperatureShiftMenuItem::AmbientTemperatureShiftMenuItem() {
		setText(L"Ambient T shift");
		setMin(0);
		setMax(255);
		setValue(Thermaller::getInstance().settings.ambientTemperatureShift);
	}

	void AmbientTemperatureShiftMenuItem::onJoystickEvent(JoystickEvent* event) {
		IntMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.ambientTemperatureShift = getValue();
		th.settings.scheduleWrite();
	}

	ChessModeMenuItem::ChessModeMenuItem() {
		setText(L"Chess mode");

		setValue(Thermaller::getInstance().settings.chessMode);
	}

	void ChessModeMenuItem::onJoystickEvent(JoystickEvent* event) {
		BoolMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::press)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.chessMode = getValue();
		th.settings.scheduleWrite();

		th.MLX.setModeFromSettings();
	}

	SensorMenuPage::SensorMenuPage() : MenuPage(L"Sensor") {
		addItems({
			&refreshRateItem,
			&ambientTemperatureShiftItem,
			&chessModeItem,
			&interpolationItem,
			&backItem
		});

		setSelectedIndex(0);
	}
}
