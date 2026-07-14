#include "UI/Menu/MenuPage.hpp"

#include <variant>
#include "Resources/Sounds.hpp"

#include <YOBA/Core.hpp>
#include <YOBA/UI.hpp>
#include "UI/Menu/MenuRoute.hpp"
#include "UI/Theme.hpp"
#include "Hardware/Joystick/Joystick.hpp"

#include "Thermaller.hpp"

namespace pizda {
	MenuPage::MenuPage(const std::string_view title) {
		// Rows
		*this += &_titleAndItemLayout;

		// Title
		_title.setFont(&Theme::fontNormal);
		_title.setTextColor(&Theme::fg1);
		_title.setText(title);

		_titleMargin.setMargin(Margin(MenuItem::padding, MenuItem::padding, 0, 10));
		_title.setLayoutTransform(&_titleMargin);

		_titleAndItemLayout.setAutoSize(&_title);
		_titleAndItemLayout += &_title;

		// ScrollView
		// _scrollView.setMargin(Margin(0, 0, 0, 8));
		// _scrollView.setScrollBarThumbColor(&Theme::bg4);
		// _scrollView.setHorizontalScrollMode(ScrollMode::disabled);
		// _titleAndItemLayout += &_scrollView;

		// Item rows
		_itemRows.setGap(4);
		// _scrollView += &_itemRows;
		_titleAndItemLayout += &_itemRows;

		// Selector
		setItemLayout(&_itemRows);
	}

	void MenuPage::addItems(const std::initializer_list<MenuItem*>& items) {
		removeItems();

		for (MenuItem* item : items)
			addItem(item);

		setSelectedIndex(0);
	}

	void MenuPage::onEventBeforeChildren(Event* event) {
		Selector::onEventBeforeChildren(event);

		const auto joystickEvent = event->as<JoystickEvent>();

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

		// if (joystickEvent->type == JoystickEventType::up || joystickEvent->type == JoystickEventType::down) {
		// 	_scrollView.scrollTo(selectedItem);
		// }

		joystickEvent->setHandled(true);
	}
}
