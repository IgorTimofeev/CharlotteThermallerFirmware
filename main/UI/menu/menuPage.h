#pragma once

#include <span>
#include <functional>
#include <UI/thermalView.h>

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/route.h"
#include "UI/theme.h"
#include "UI/menu/menuItem.h"
#include "hardware/joystick/joystick.h"

namespace pizda {
	class MenuPage : public StackLayout {
		public:
			MenuPage();

			void addItems(const std::initializer_list<MenuItem*>& items);

			uint8_t getSelectedIndex() const;

			void setSelectedIndex(const uint8_t index);

		protected:
			void onEventBeforeChildren(Event* event) override;

		private:
			uint8_t _selectedIndex = 0;

			void updateSelection() const;
	};
}