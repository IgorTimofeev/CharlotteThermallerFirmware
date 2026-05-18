#pragma once

#include <span>
#include <functional>
#include <UI/thermalView.h>

#include <YOBA/main.h>
#include <YOBA/UI.h>

#include "UI/route.h"
#include "UI/theme.h"
#include "hardware/joystick/joystick.h"

namespace pizda {
	class MenuItem : public Control, public ActiveElement, public TextElement, public TextColorElement {
		public:
			MenuItem();

			virtual void onJoystickEvent(JoystickEvent* event);

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;
	};

	class RouteMenuItem : public MenuItem {
		public:
			RouteMenuItem(const std::wstring_view text, const Route route);

			Route getRoute() const;

			void onJoystickEvent(JoystickEvent* event) override;

		private:
			const Route _route;
	};

	class IntMenuItem : public MenuItem {
		public:
			void onJoystickEvent(JoystickEvent* event) override;

			int32_t getMin() const;
			void setMin(const int32_t min);

			int32_t getMax() const;
			void setMax(const int32_t max);

			int32_t getStep() const;
			void setStep(const int32_t step);

			int32_t getValue() const;
			void setValue(const int32_t value);

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;

		private:
			int32_t _min = std::numeric_limits<int32_t>::min();
			int32_t _max = std::numeric_limits<int32_t>::max();
			int32_t _step = 1;
			int32_t _value = 0;
	};

	class BoolMenuItem : public MenuItem {
		public:
			void onJoystickEvent(JoystickEvent* event) override;

			bool getValue() const;
			void setValue(const bool value);

		protected:
			void onRender(Renderer* renderer, const Bounds& bounds) override;

		private:
			bool _value = false;
	};
}