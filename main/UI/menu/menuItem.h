#pragma once

#include <span>
#include <functional>
#include <UI/thermalView.h>

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "menuRoute.h"
#include "UI/theme.h"
#include "hardware/joystick/joystick.h"

namespace pizda {
	class MenuItem : public SelectorItem, public TextElement, public TextColorElement {
		public:
			MenuItem();

			constexpr static uint8_t padding = 18;

			virtual void onJoystickEvent(JoystickEvent* event);

		protected:
			void onRender(Renderer* renderer, const Rectangle& bounds) override;

			void renderRightText(Renderer* renderer, const Rectangle& bounds, const wchar_t* text) const;
			void renderRightTextWithArrows(Renderer* renderer, const Rectangle& bounds,
			                              const wchar_t* text, bool leftEnabled, bool rightEnabled);
			void renderRightCircle(Renderer* renderer, const Rectangle& bounds, const Color* color) const;
	};

	class RouteMenuItem : public MenuItem {
		public:
			RouteMenuItem(const std::wstring_view text, const MenuRoute route);

			MenuRoute getRoute() const;

			void onJoystickEvent(JoystickEvent* event) override;

		private:
			const MenuRoute _route;
	};

	class IntMenuItem : public MenuItem {
		public:
			void onJoystickEvent(JoystickEvent* event) override;

			int32_t getMin() const;
			void setMin(const int32_t min);

			int32_t getMax() const;
			void setMax(const int32_t max);

			int32_t getSmallStep() const;
			void setSmallStep(const int32_t step);

			int32_t getBigStep() const;
			void setBigStep(int32_t step);

			int32_t getValue() const;
			void setValue(const int32_t value);

		protected:
			void onRender(Renderer* renderer, const Rectangle& bounds) override;

		private:
			int32_t _min = std::numeric_limits<int32_t>::min();
			int32_t _max = std::numeric_limits<int32_t>::max();
			int32_t _smallStep = 1;
			int32_t _bigStep = 5;
			int32_t _value = 0;
	};

	class VariantMenuItem : public MenuItem {
		public:
			void onJoystickEvent(JoystickEvent* event) override;

			int32_t getVariantIndex() const;
			void setVariantIndex(const uint8_t value);

			int32_t getVariantCount() const;
			void setVariantCount(const uint8_t value);

		protected:
			virtual const wchar_t* variantToString() = 0;

			void onRender(Renderer* renderer, const Rectangle& bounds) override;

		private:
			uint8_t _variantIndex = 0;
			uint8_t _variantCount = 0;
	};

	class BoolMenuItem : public MenuItem {
		public:
			void onJoystickEvent(JoystickEvent* event) override;

			bool getValue() const;
			void setValue(const bool value);

		protected:
			void onRender(Renderer* renderer, const Rectangle& bounds) override;

		private:
			bool _value = false;
	};
}