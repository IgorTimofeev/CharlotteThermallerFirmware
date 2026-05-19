#include "UI/menu/menuPage.h"

#include <variant>

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include "UI/route.h"
#include "UI/theme.h"
#include "hardware/joystick/joystick.h"

#include "thermaller.h"

namespace pizda {
	MenuPage::MenuPage() {
		_scrollView.setScrollBarThumbColor(&Theme::bg5);
		_scrollView.setHorizontalScrollMode(ScrollMode::disabled);
		*this += &_scrollView;

		_rows.setGap(2);
		_scrollView += &_rows;

		setItemsLayout(&_rows);
	}

	void MenuPage::addItems(const std::initializer_list<MenuItem*>& items) {
		removeItems();

		for (MenuItem* item : items)
			addItem(item);

		setSelectedIndex(0);
	}

	void MenuPage::onEventBeforeChildren(Event* event) {
		Selector::onEventBeforeChildren(event);

		const auto joystickEvent = event->castTo<JoystickEvent>();

		if (!joystickEvent || getItemCount() == 0)
			return;

		auto selectedIndex = getSelectedIndex();

		if (joystickEvent->type == JoystickEventType::up) {
			if (selectedIndex == 0) {
				selectedIndex = getItemCount() - 1;
			}
			else {
				selectedIndex--;
			}

			setSelectedIndex(selectedIndex);
		}
		else if (joystickEvent->type == JoystickEventType::down) {
			if (selectedIndex >= getItemCount() - 1) {
				selectedIndex = 0;
			}
			else {
				selectedIndex++;
			}

			setSelectedIndex(selectedIndex);
		}

		const auto selectedItem = dynamic_cast<MenuItem*>(getItemAt(selectedIndex));
		selectedItem->onJoystickEvent(joystickEvent);

		if (joystickEvent->type == JoystickEventType::up || joystickEvent->type == JoystickEventType::down)
			_scrollView.scrollTo(selectedItem);

		joystickEvent->setHandled(true);
	}
}
