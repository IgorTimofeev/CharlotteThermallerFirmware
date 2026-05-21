#include "UI/menu/menuItem.h"

#include <span>
#include <functional>
#include <resources/sounds.h>

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "menuRoute.h"
#include "UI/theme.h"
#include "hardware/joystick/joystick.h"

#include "thermaller.h"

namespace pizda {
	MenuItem::MenuItem() {
		setHeight(26);
		setTextColor(&Theme::fg4);
	}

	void MenuItem::onJoystickEvent(JoystickEvent* event) {

	}

	void MenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		if (isActive())
			renderer->renderFilledRectangle(bounds, &Theme::fg1);

		const auto textColor = isActive() ? &Theme::bg1 : getTextColor();

		renderer->renderString(
			Point(bounds.getX() + padding, bounds.getYCenter() - Theme::fontNormal.getHeight() / 2),
			&Theme::fontNormal,
			textColor,
			getText()
		);
	}

	void MenuItem::renderRightInt(Renderer* renderer, const Bounds& bounds, const int32_t value) const {
		wchar_t text[16] {};
		std::swprintf(text, 16, L"%d", value);

		renderer->renderString(
			Point(
				bounds.getX2() - padding - Theme::fontNormal.getWidth(text),
				bounds.getYCenter() - Theme::fontNormal.getHeight() / 2
			),
			&Theme::fontNormal,
			isActive() ? &Theme::bg1 : &Theme::fg7,
			text
		);
	}

	void MenuItem::renderRightCircle(Renderer* renderer, const Bounds& bounds, const Color* color) const {
		renderer->renderFilledCircle(
			Point(bounds.getX2() - padding - 3, bounds.getYCenter()),
			3,
			isActive() ? &Theme::bg1 : color
		);
	}

	RouteMenuItem::RouteMenuItem(const std::wstring_view text, const MenuRoute route) : _route(route) {
		setText(text);
	}

	MenuRoute RouteMenuItem::getRoute() const {
		return _route;
	}

	void RouteMenuItem::onJoystickEvent(JoystickEvent* event) {
		if (event->type != JoystickEventType::press)
			return;

		auto& th = Thermaller::getInstance();

		th.setRoute(_route);

		th.audioPlayer.play(&resources::sounds::feedback);
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

	int32_t IntMenuItem::getSmallStep() const {
		return _smallStep;
	}

	void IntMenuItem::setSmallStep(const int32_t step) {
		_smallStep = step;
	}

	int32_t IntMenuItem::getBigStep() const {
		return _bigStep;
	}

	void IntMenuItem::setBigStep(const int32_t step) {
		_bigStep = step;
	}

	int32_t IntMenuItem::getValue() const {
		return _value;
	}

	void IntMenuItem::setValue(const int32_t value) {
		_value = value;
	}

	void IntMenuItem::onJoystickEvent(JoystickEvent* event) {
		MenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		const auto step = event->razyob ? _bigStep : _smallStep;

		_value = std::clamp(_value + (event->type == JoystickEventType::left ? -step : step), _min, _max);

		invalidate();

		Thermaller::getInstance().audioPlayer.play(&resources::sounds::feedback);
	}

	void IntMenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		MenuItem::onRender(renderer, bounds);

		constexpr static uint8_t triangleWidth = 4;
		constexpr static uint8_t triangleHeight = 7;
		constexpr static uint8_t triangleMargin = 8;

		if (isActive()) {
			int32_t x = bounds.getX2() - padding;
			const auto yCenter = bounds.getYCenter();

			// Right triangle
			renderer->renderFilledTriangle(
				Point(x, yCenter),
				Point(x - triangleWidth, yCenter - triangleHeight / 2),
				Point(x - triangleWidth, yCenter + triangleHeight / 2),
				_value < _max ? &Theme::bg1 : &Theme::fg4
			);

			x -= triangleWidth + triangleMargin;

			// Text
			wchar_t text[16] {};
			std::swprintf(text, 16, L"%d", _value);
			const auto textWidth = Theme::fontNormal.getWidth(text);

			renderer->renderString(
				Point(
					x - textWidth,
					yCenter - Theme::fontNormal.getHeight() / 2
				),
				&Theme::fontNormal,
				isActive() ? &Theme::bg1 : &Theme::fg7,
				text
			);

			x -= textWidth + triangleMargin;

			// Left triangle
			renderer->renderFilledTriangle(
				Point(x - triangleWidth, yCenter),
				Point(x, yCenter - triangleHeight / 2),
				Point(x, yCenter + triangleHeight / 2),
				_value > _min ? &Theme::bg1 : &Theme::fg4
			);
		}
		else {
			renderRightInt(renderer, bounds, _value);
		}
	}

	void BoolMenuItem::onJoystickEvent(JoystickEvent* event) {
		if (event->type != JoystickEventType::press)
			return;

		_value = !_value;

		invalidate();

		Thermaller::getInstance().audioPlayer.play(&resources::sounds::feedback);
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

		renderRightCircle(renderer, bounds, &Theme::green);
	}
}
