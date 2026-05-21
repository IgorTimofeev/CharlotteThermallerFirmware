#include "UI/menu/menuPage.h"

#include <variant>
#include <resources/sounds.h>

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include "menuRoute.h"
#include "UI/theme.h"
#include "hardware/joystick/joystick.h"

#include "thermaller.h"

namespace pizda {
	MenuPage::MenuPage(const std::wstring_view title) {
		// Rows
		*this += &_titleAndScrollViewerLayout;

		// Title
		_title.setMargin(Margin(MenuItem::padding, MenuItem::padding, 0, 10));
		_title.setFont(&Theme::fontNormal);
		_title.setTextColor(&Theme::fg1);
		_title.setText(title);
		_titleAndScrollViewerLayout.setAutoSize(&_title);
		_titleAndScrollViewerLayout += &_title;

		// ScrollView
		_scrollView.setMargin(Margin(0, 0, 0, 8));
		_scrollView.setScrollBarThumbColor(&Theme::bg4);
		_scrollView.setHorizontalScrollMode(ScrollMode::disabled);
		_titleAndScrollViewerLayout += &_scrollView;

		// Item rows
		_itemRows.setGap(4);
		_scrollView += &_itemRows;

		// Selector
		setItemsLayout(&_itemRows);
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

		if (joystickEvent->type == JoystickEventType::up || joystickEvent->type == JoystickEventType::down) {
			setSelectedIndex(
				joystickEvent->type == JoystickEventType::up
				? (
					getSelectedIndex() == 0
					? getItemCount() - 1
					: getSelectedIndex() - 1
				)
				: (
					getSelectedIndex() >= getItemCount() - 1
					? 0
					: getSelectedIndex() + 1
				)
			);

			Thermaller::getInstance().audioPlayer.play(&resources::sounds::feedback);
		}

		const auto selectedItem = dynamic_cast<MenuItem*>(getItemAt(getSelectedIndex()));
		selectedItem->onJoystickEvent(joystickEvent);

		if (joystickEvent->type == JoystickEventType::up || joystickEvent->type == JoystickEventType::down) {
			_scrollView.scrollTo(selectedItem);
		}

		joystickEvent->setHandled(true);
	}
}
