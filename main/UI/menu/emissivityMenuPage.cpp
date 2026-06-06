#include "UI/menu/emissivityMenuPage.h"

#include <resources/sounds.h>

#include "thermaller.h"

namespace pizda {
	EmissivityMenuItem::EmissivityMenuItem() {
		setText("Value");
		setMin(0);
		setMax(100);
		setSmallStep(1);
		setBigStep(5);
		setValue(Thermaller::getInstance().settings.emissivityPercent);
	}

	void EmissivityMenuItem::onJoystickEvent(JoystickEvent* event) {
		IntMenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.emissivityPercent = getValue();
		th.settings.writeLater();
	}

	EmissivityPresetMenuItem::EmissivityPresetMenuItem(const std::string_view title, const uint8_t emissivity): _emissivity(emissivity) {
		setText(title);
	}

	void EmissivityPresetMenuItem::onJoystickEvent(JoystickEvent* event) {
		if (event->type != JoystickEventType::press)
			return;

		auto& th = Thermaller::getInstance();
		th.settings.emissivityPercent = _emissivity;
		th.settings.writeLater();

		const auto menuPage = dynamic_cast<EmissivityMenuPage*>(getSelector());
		menuPage->manualItem.setValue(th.settings.emissivityPercent);

		th.audioPlayer.play(&resources::sounds::feedback);

		invalidate();
	}

	void EmissivityPresetMenuItem::onRender(Renderer* renderer, const Rectangle& bounds) {
		MenuItem::onRender(renderer, bounds);

		if (Thermaller::getInstance().settings.emissivityPercent == _emissivity)
			renderRightCircle(renderer, bounds, &Theme::green);
	}

	EmissivityMenuPage::EmissivityMenuPage() : MenuPage("Emissivity") {
		addItems({
			&manualItem,
			&matteItem,
			&semiGlossItem,
			&glossyItem,
			&reflectiveItem,
			&backItem
		});

		setSelectedIndex(0);
	}
}
