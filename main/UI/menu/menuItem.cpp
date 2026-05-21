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

	void MenuItem::renderRightText(Renderer* renderer, const Bounds& bounds, const wchar_t* text) const {
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

	void MenuItem::renderRightTextWithArrows(Renderer* renderer, const Bounds& bounds, const wchar_t* text, const bool leftEnabled, const bool rightEnabled) {
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
				rightEnabled ? &Theme::bg1 : &Theme::fg2
			);

			x -= triangleWidth + triangleMargin;

			// Text
			const auto textWidth = Theme::fontNormal.getWidth(text);

			renderer->renderString(
				Point(
					x - textWidth,
					yCenter - Theme::fontNormal.getHeight() / 2
				),
				&Theme::fontNormal,
				&Theme::bg1,
				text
			);

			x -= textWidth + triangleMargin;

			// Left triangle
			renderer->renderFilledTriangle(
				Point(x - triangleWidth, yCenter),
				Point(x, yCenter - triangleHeight / 2),
				Point(x, yCenter + triangleHeight / 2),
				leftEnabled ? &Theme::bg1 : &Theme::fg2
			);
		}
		else {
			renderRightText(renderer, bounds, text);
		}
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

		invalidate();
	}

	int32_t IntMenuItem::getMax() const {
		return _max;
	}

	void IntMenuItem::setMax(const int32_t max) {
		_max = max;

		invalidate();
	}

	int32_t IntMenuItem::getSmallStep() const {
		return _smallStep;
	}

	void IntMenuItem::setSmallStep(const int32_t step) {
		_smallStep = step;

		invalidate();
	}

	int32_t IntMenuItem::getBigStep() const {
		return _bigStep;
	}

	void IntMenuItem::setBigStep(const int32_t step) {
		_bigStep = step;

		invalidate();
	}

	int32_t IntMenuItem::getValue() const {
		return _value;
	}

	void IntMenuItem::setValue(const int32_t value) {
		_value = value;

		invalidate();
	}

	void IntMenuItem::onJoystickEvent(JoystickEvent* event) {
		MenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		const auto step = event->razyob ? _bigStep : _smallStep;

		setValue(std::clamp(_value + (event->type == JoystickEventType::right ? step : -step), _min, _max));

		Thermaller::getInstance().audioPlayer.play(&resources::sounds::feedback);
	}

	void IntMenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		MenuItem::onRender(renderer, bounds);

		wchar_t text[16] {};
		std::swprintf(text, 16, L"%d", _value);

		renderRightTextWithArrows(
			renderer,
			bounds,
			text,
			_value > _min,
			_value < _max
		);
	}

	int32_t VariantMenuItem::getVariantIndex() const {
		return _variantIndex;
	}

	void VariantMenuItem::setVariantIndex(const uint8_t value) {
		_variantIndex = value;

		invalidate();
	}

	int32_t VariantMenuItem::getVariantCount() const {
		return _variantIndex;
	}

	void VariantMenuItem::setVariantCount(const uint8_t value) {
		_variantCount = value;

		invalidate();
	}

	void VariantMenuItem::onJoystickEvent(JoystickEvent* event) {
		MenuItem::onJoystickEvent(event);

		if (event->type != JoystickEventType::left && event->type != JoystickEventType::right)
			return;

		setVariantIndex(std::clamp(_variantIndex + (event->type == JoystickEventType::right ? 1 : -1), 0, _variantCount - 1));

		Thermaller::getInstance().audioPlayer.play(&resources::sounds::feedback);
	}

	void VariantMenuItem::onRender(Renderer* renderer, const Bounds& bounds) {
		MenuItem::onRender(renderer, bounds);

		renderRightTextWithArrows(
			renderer,
			bounds,
			variantToString(),
			_variantIndex > 0,
			_variantIndex < _variantCount - 1
		);
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
