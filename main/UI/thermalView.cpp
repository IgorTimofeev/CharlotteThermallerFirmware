#include "thermalView.h"
#include "UI/theme.h"

namespace pizda {
	ThermalView::ThermalView() {

	}

	void ThermalView::onRender(Renderer* renderer, const Bounds& bounds) {
		renderer->renderFilledRectangle(Bounds(5, 5, 30, 10), &Theme::red);
	}
}