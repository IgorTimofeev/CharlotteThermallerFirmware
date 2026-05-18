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
	class MenuItem : public Control, public ActiveElement, public TextElement {
		public:
			MenuItem(const std::wstring_view text);

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
}