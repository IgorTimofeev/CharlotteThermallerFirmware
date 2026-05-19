#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "theme.h"

namespace pizda {
	using namespace YOBA;

	enum class ThermalPalette : uint8_t {
		hunting,
		ironbow,
		rainbow,
		arctic,
		whiteHot,
		blackHot
	};

	class ThermalView : public Control {
		public:
			ThermalView();

		protected:
			void onTick() override;
			void onEvent(Event* event) override;
			void onRender(Renderer* renderer, const Bounds& bounds) override;

		private:
			constexpr static auto _font = &Theme::fontSmall;
			constexpr static uint8_t _fontScale = 2;
			constexpr static uint8_t _shadowOffset = 1;

			float _tCross = 0;

			static void renderShadowedText(Renderer* renderer, const Point& position, std::wstring_view text);
	};
}
