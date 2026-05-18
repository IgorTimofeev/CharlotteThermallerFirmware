#include "UI/menu/paletteMenuPage.h"

#include "thermaller.h"

namespace pizda {
	PaletteMenuItem::PaletteMenuItem(const std::wstring_view text, const ThermalPalette palette) : _palette(palette) {
		setText(text);
	}

	ThermalPalette PaletteMenuItem::getRoute() const {
		return _palette;
	}

	void PaletteMenuItem::onJoystickEvent(JoystickEvent* event) {
		if (event->type != JoystickEventType::press)
			return;

		auto& th = Thermaller::getInstance();

		th.settings.thermalPalette = _palette;
		th.settings.scheduleWrite();

		invalidate();
	}

	void PaletteMenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		MenuItem::onRender(renderer, bounds);

		if (Thermaller::getInstance().settings.thermalPalette == _palette) {
			renderer->renderFilledCircle(
				Point(bounds.getX(), bounds.getYCenter()),
				4,
				isActive() ? &Theme::bg1 : &Theme::green
			);
		}
	}

	PaletteMenuPage::PaletteMenuPage() {
		addItems({
			&govnoItem,
			&ironbowItem,
			&whiteHotItem,
			&backItem
		});

		setSelectedIndex(0);
	}
}
