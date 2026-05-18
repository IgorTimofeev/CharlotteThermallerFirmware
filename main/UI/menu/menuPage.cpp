#include "UI/menu/menuPage.h"

#include <span>
#include <functional>

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/route.h"
#include "UI/theme.h"
#include "hardware/joystick/joystick.h"

#include "thermaller.h"

namespace pizda {
	MenuPage::MenuPage() {
		setGap(1);
	}

	void MenuPage::addItems(const std::initializer_list<MenuItem*>& items) {
		removeChildren();

		for (MenuItem* item : items) {
			*this += item;
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
		StackLayout::onEventBeforeChildren(event);

		if (event->getTypeID() != JoystickEvent::typeID)
			return;

		const auto joystickEvent = static_cast<JoystickEvent*>(event);

		if (getChildrenCount() == 0)
			return;

		if (joystickEvent->type == JoystickEventType::up) {
			if (_selectedIndex == 0) {
				_selectedIndex = getChildrenCount() - 1;
			}
			else {
				_selectedIndex--;
			}

			updateSelection();
		}
		else if (joystickEvent->type == JoystickEventType::down) {
			if (_selectedIndex >= getChildrenCount() - 1) {
				_selectedIndex = 0;
			}
			else {
				_selectedIndex++;
			}

			updateSelection();
		}

		const auto selectedItem = dynamic_cast<MenuItem*>(getChildAt(_selectedIndex));
		selectedItem->onJoystickEvent(joystickEvent);

		joystickEvent->setHandled(true);
	}

	void MenuPage::updateSelection() const {
		for (uint8_t i = 0; i < getChildrenCount(); i++) {
			const auto menuItem = dynamic_cast<MenuItem*>(getChildAt(i));

			menuItem->setActive(i == _selectedIndex);
		}
	}
}
