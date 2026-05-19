#pragma once

#include <YOBA/UI.h>

#include "UI/menu/menuItem.h"

namespace pizda {
	class MenuPage : public Selector {
		public:
			MenuPage();

			void addItems(const std::initializer_list<MenuItem*>& items);

		protected:
			void onEventBeforeChildren(Event* event) override;

		private:
			ScrollView _scrollView {};
			StackLayout _rows {};
	};
}