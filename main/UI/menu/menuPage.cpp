#include "UI/menu/menuPage.h"

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include "UI/route.h"
#include "UI/theme.h"
#include "hardware/joystick/joystick.h"

#include "thermaller.h"

namespace pizda {
	MenuPage::MenuPage() {
		setScrollBarThumbColor(&Theme::bg5);
		setHorizontalScrollMode(ScrollMode::disabled);

		rows.setGap(2);
		*this += &rows;
	}

	void MenuPage::addItems(const std::initializer_list<MenuItem*>& items) {
		rows.removeChildren();

		for (MenuItem* item : items) {
			rows += item;
		}

		updateSelection();
	}

	uint8_t MenuPage::getSelectedIndex() const {
		return _selectedIndex;
	}

	void MenuPage::setSelectedIndex(const uint8_t index) {
		_selectedIndex = index;

		updateSelection();
	}

	void MenuPage::onEventBeforeChildren(Event* event) {
		ScrollView::onEventBeforeChildren(event);

		if (event->getTypeID() != JoystickEvent::typeID)
			return;

		const auto joystickEvent = static_cast<JoystickEvent*>(event);

		if (rows.getChildrenCount() == 0)
			return;

		if (joystickEvent->type == JoystickEventType::up) {
			if (_selectedIndex == 0) {
				_selectedIndex = rows.getChildrenCount() - 1;
			}
			else {
				_selectedIndex--;
			}

			updateSelection();
		}
		else if (joystickEvent->type == JoystickEventType::down) {
			if (_selectedIndex >= rows.getChildrenCount() - 1) {
				_selectedIndex = 0;
			}
			else {
				_selectedIndex++;
			}

			updateSelection();
		}

		const auto selectedItem = dynamic_cast<MenuItem*>(rows.getChildAt(_selectedIndex));
		selectedItem->onJoystickEvent(joystickEvent);

		if (joystickEvent->type == JoystickEventType::up || joystickEvent->type == JoystickEventType::down)
			scrollTo(selectedItem);

		joystickEvent->setHandled(true);
	}

	void MenuPage::updateSelection() const {
		for (uint8_t i = 0; i < rows.getChildrenCount(); i++) {
			const auto menuItem = dynamic_cast<MenuItem*>(rows.getChildAt(i));

			menuItem->setActive(i == _selectedIndex);
		}
	}
}
