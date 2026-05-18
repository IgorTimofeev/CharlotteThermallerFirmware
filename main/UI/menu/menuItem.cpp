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
	MenuItem::MenuItem() {
		setHeight(26);
		setTextColor(&Theme::fg3);
	}

	void MenuItem::onJoystickEvent(JoystickEvent* event) {

	}

	void MenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		if (isActive())
			renderer->renderFilledRectangle(bounds, &Theme::fg1);

		const auto textColor = isActive() ? &Theme::bg1 : getTextColor();

		renderer->renderString(
			Point(bounds.getX() + 20, bounds.getYCenter() - Theme::fontNormal.getHeight() / 2),
			&Theme::fontNormal,
			textColor,
			getText()
		);
	}

	RouteMenuItem::RouteMenuItem(const std::wstring_view text, const Route route) : _route(route) {
		setText(text);
	}

	Route RouteMenuItem::getRoute() const {
		return _route;
	}

	void RouteMenuItem::onJoystickEvent(JoystickEvent* event) {
		if (event->type != JoystickEventType::press)
			return;

		Thermaller::getInstance().setRoute(_route);
	}

	int32_t IntMenuItem::getMin() const {
		return _min;
	}

	void IntMenuItem::setMin(const int32_t min) {
		_min = min;
	}

	int32_t IntMenuItem::getMax() const {
		return _max;
	}

	void IntMenuItem::setMax(const int32_t max) {
		_max = max;
	}

	int32_t IntMenuItem::getStep() const {
		return _step;
	}

	void IntMenuItem::setStep(const int32_t step) {
		_step = step;
	}

	int32_t IntMenuItem::getValue() const {
		return _value;
	}

	void IntMenuItem::setValue(const int32_t value) {
		_value = value;
	}

	void IntMenuItem::onJoystickEvent(JoystickEvent* event) {
		MenuItem::onJoystickEvent(event);

		if (event->type == JoystickEventType::right) {
			_value = std::clamp(_value + _step, _min, _max);

			invalidate();
		}
		else if (event->type == JoystickEventType::left) {
			_value = std::clamp(_value - _step, _min, _max);

			invalidate();
		}
	}

	void IntMenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		MenuItem::onRender(renderer, bounds);

		wchar_t text[16] {};
		std::swprintf(text, 16, L"%d", _value);

		renderer->renderString(
			Point(
				bounds.getX2() - 20 - Theme::fontNormal.getWidth(text),
				bounds.getYCenter() - Theme::fontNormal.getHeight() / 2
			),
			&Theme::fontNormal,
			isActive() ? &Theme::bg1 : &Theme::bg5,
			text
		);
	}

	void BoolMenuItem::onJoystickEvent(JoystickEvent* event) {
		if (event->type != JoystickEventType::press)
			return;

		_value = !_value;

		invalidate();
	}

	bool BoolMenuItem::getValue() const {
		return _value;
	}

	void BoolMenuItem::setValue(const bool value) {
		_value = value;
	}

	void BoolMenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		MenuItem::onRender(renderer, bounds);

		if (!_value)
			return;

		renderer->renderFilledCircle(
			Point(bounds.getX2() - 20, bounds.getYCenter()),
			4,
			isActive() ? &Theme::bg1 : &Theme::green
		);
	}
}
