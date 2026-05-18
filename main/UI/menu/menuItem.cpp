#include "UI/menu/menuItem.h"

#include <span>
#include <functional>

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/route.h"
#include "UI/theme.h"
#include "hardware/joystick/joystick.h"

#include "thermaller.h"

namespace pizda {
	MenuItem::MenuItem(const std::wstring_view text) {
		setHeight(26);

		setText(text);
	}

	void MenuItem::onJoystickEvent(JoystickEvent* event) {

	}

	void MenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		if (isActive()) {
			renderer->renderFilledRectangle(bounds, &Theme::fg1);
		}

		const auto textColor = isActive() ? &Theme::fg1 : &Theme::bg1;

		renderer->renderString(
			Point(bounds.getX() + 20, bounds.getYCenter() - Theme::fontNormal.getHeight() / 2),
			&Theme::fontNormal,
			textColor,
			getText()
		);
	}

	RouteMenuItem::RouteMenuItem(const std::wstring_view text, const Route route): MenuItem(text), _route(route) {

	}

	Route RouteMenuItem::getRoute() const {
		return _route;
	}

	void RouteMenuItem::onJoystickEvent(JoystickEvent* event) {
		if (event->type != JoystickEventType::press)
			return;

		Thermaller::getInstance().setRoute(_route);
	}
}
