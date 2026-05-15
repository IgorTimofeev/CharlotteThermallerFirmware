#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>

namespace pizda {
	using namespace YOBA;

	class ThermalView : public Control {
		public:
			ThermalView();

		protected:
			void onTick() override;
			void onRender(Renderer* renderer, const Bounds& bounds) override;
	};
}
