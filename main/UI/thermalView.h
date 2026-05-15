#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "theme.h"

namespace pizda {
	using namespace YOBA;

	class ThermalView : public Control {
		public:
			ThermalView();

		protected:
			void onTick() override;
			void onRender(Renderer* renderer, const Bounds& bounds) override;

		private:
			constexpr static auto _font = &Theme::fontNormal;
			constexpr static auto _palette = &Theme::thermalPaletteIronbow;

			constexpr static uint8_t _shadowOffset = 1;

			bool _histogramLPFProcessed = false;
			float _hMin = 0;
			float _hMax = 0;
			float _tCross = 0;

			static void renderShadowedText(Renderer* renderer, const Point& position, std::wstring_view text);
	};
}
