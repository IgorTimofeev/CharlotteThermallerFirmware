#pragma once

#include <YOBA/Core.hpp>
#include <YOBA/UI.hpp>

#include "Theme.hpp"

namespace pizda {
	using namespace YOBA;

	enum class ThermalPalette : uint8_t {
		hunting,
		ironbow,
		arctic,
		whiteHot,
		blackHot
	};

	class ThermalView : public Control {
		protected:
			void onTick() override;
			void onEvent(Event* event) override;
			void onRender(Renderer* renderer, const Rectangle& bounds) override;

		private:
			constexpr static auto _font = &Theme::fontSmall;
			constexpr static uint8_t _fontScale = 2;
			constexpr static uint8_t _shadowOffset = 1;

			static void renderShadowedText(Renderer* renderer, const Point& position, std::string_view text);
	};
}
