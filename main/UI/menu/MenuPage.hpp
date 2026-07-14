#pragma once

#include <YOBA/UI.hpp>

#include "UI/Menu/MenuItem.hpp"

namespace pizda {
	class MenuPage : public Selector {
		public:
			MenuPage(const std::string_view title);
			void addItems(const std::initializer_list<MenuItem*>& items);

		protected:
			void onEventBeforeChildren(Event* event) override;

		private:
			// ScrollView _scrollView {};
			RelativeStackLayout _titleAndItemLayout {};

			MarginTransform _titleMargin {};
			TextView _title {};

			StackLayout _itemRows {};
	};
}