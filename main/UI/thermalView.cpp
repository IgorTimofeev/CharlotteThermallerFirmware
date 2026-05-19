#include "thermalView.h"

#include <ranges>
#include <span>

#include "thermaller.h"
#include "UI/theme.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lowPassFilter.h>

#include "hardware/MLX90640/MLX90640.h"
#include "hardware/joystick/joystick.h"

namespace pizda {
	ThermalView::ThermalView() {

	}

	void ThermalView::onTick() {
		invalidate();
	}

	void ThermalView::onEvent(Event* event) {
		Control::onEvent(event);

		const auto joystickEvent = event->castTo<JoystickEvent>();

		if (!joystickEvent)
			return;

		if (joystickEvent->type == JoystickEventType::press) {
			auto& th = Thermaller::getInstance();

			th.setRoute(Route::main);
		}

		event->setHandled(true);
	}

	void ThermalView::onRender(Renderer* renderer, const Bounds& bounds) {
		auto& th = Thermaller::getInstance();

		std::array<float, th.MLX.frame.size()> frame {};

		xSemaphoreTake(th.MLX.frameMutex, portMAX_DELAY);
		std::ranges::copy(th.MLX.frame, frame.begin());
		xSemaphoreGive(th.MLX.frameMutex);
		
		const auto x2 = bounds.getX2();
		const auto y2 = bounds.getY2();
		const auto center = bounds.getCenter();

		std::span<const RGB565Color> palette;

		switch (th.settings.thermalPalette) {
			case ThermalPalette::hunting:
				palette = { Theme::thermalPaletteHunting };
				break;
			case ThermalPalette::ironbow:
				palette = { Theme::thermalPaletteIronbow };
				break;
			case ThermalPalette::rainbow:
				palette = { Theme::thermalPaletteRainbow };
				break;
			case ThermalPalette::arctic:
				palette = { Theme::thermalPaletteArctic };
				break;
			case ThermalPalette::whiteHot:
				palette = { Theme::thermalPaletteWhiteHot };
				break;
			case ThermalPalette::blackHot:
				palette = { Theme::thermalPaletteBlackHot };
				break;
		}

		float hMin  = 0;
		float hMax  = 0;
		float tAvg = 0;

		{
			float tMin = 99999;
			float tMax = -99999;

			for (uint16_t i = 0; i < frame.size(); ++i) {
				if (std::isnan(frame[i]))
					continue;

				tMin = std::min(tMin, frame[i]);
				tMax = std::max(tMax, frame[i]);
				tAvg += frame[i];
			}

			tAvg /= frame.size();

			if (th.settings.rangeAuto) {
				hMin = tMin;
				hMax = tMax;
			}
			else {
				hMin = th.settings.rangeMin;
				hMax = th.settings.rangeMax;
			}
		}

		const float tMinMaxDelta = std::max<float>(hMax - hMin, 1);

		// ESP_LOGI("afa", "tmin = %f, tmax = %f, tavg = %f", tMin, tMax, tAvg);

		const uint16_t pSize = bounds.getWidth() / MLX90640::frameHeight;

		const auto getTAt = [&frame](const uint8_t x, const uint8_t y) -> float {
			return frame[y * MLX90640::frameWidth + x];
		};

		const auto getColor = [tMinMaxDelta, tAvg, hMin, hMax, palette](float t) -> const RGB565Color* {
			if (std::isnan(t))
				return &Theme::bg1;

			t = std::clamp(std::isnan(t) ? tAvg : t, hMin, hMax);

			const auto ratio = (t - hMin) / tMinMaxDelta;
			auto index = static_cast<uint16_t>(std::round(static_cast<float>(palette.size()) * ratio));

			if (index >= palette.size())
				index = palette.size() - 1;

			return &palette[index];
		};

		if (th.settings.interpolation) {
			for (uint8_t tY = 0; tY < MLX90640::frameHeight; ++tY) {
				const int32_t pX = x2 - tY * pSize;

				for (uint8_t tX = 0; tX < MLX90640::frameWidth; ++tX) {
					const int32_t pY = y2 - tX * pSize;

					const uint8_t tX1 = std::min<uint8_t>(tX + 1, MLX90640::frameWidth - 1);
					const uint8_t tY1 = std::min<uint8_t>(tY + 1, MLX90640::frameHeight - 1);

					const float t11 = getTAt(tX, tY);
					const float t12 = getTAt(tX, tY1);
					const float t21 = getTAt(tX1, tY);
					const float t22 = getTAt(tX1, tY1);

					for (uint8_t iY = 0; iY < pSize; ++iY) {
						const float ratioX = static_cast<float>(iY) / static_cast<float>(pSize);

						for (uint8_t iX = 0; iX < pSize; ++iX) {
							const float ratioY = static_cast<float>(iX) / static_cast<float>(pSize);

							const float tI =
								(1 - ratioX) * (1 - ratioY) * t11
								+ ratioX * (1 - ratioY) * t21
								+ (1 - ratioX) * ratioY * t12
								+ ratioX * ratioY * t22;

							renderer->renderPixel(Point(pX - iX, pY - iY), getColor(tI));
						}
					}
				}
			}
		}
		else {
			Bounds pBounds { 0, 0, pSize, pSize};

			for (uint8_t tY = 0; tY < MLX90640::frameHeight; ++tY) {
				pBounds.setX(x2 - pSize - tY * pSize);

				for (uint8_t tX = 0; tX < MLX90640::frameWidth; ++tX) {
					pBounds.setY(y2 - pSize - tX * pSize);

					renderer->renderFilledRectangle(pBounds, getColor(getTAt(tX, tY)));
				}
			}
		}

		// Cross
		{
			constexpr static uint8_t crossThickness = 2;
			constexpr static uint8_t crossLength = crossThickness * 4;

			const auto renderCross = [renderer](const Point& position, const Color* color) {
				renderer->renderFilledRectangle(
					{
						position.getX() - crossLength / 2,
						position.getY() - crossThickness / 2,
						crossLength,
						crossThickness
					},
					color
				);

				renderer->renderFilledRectangle(
					{
						position.getX() - crossThickness / 2,
						position.getY() - crossLength / 2,
						crossThickness,
						crossLength
					},
					color
				);
			};

			renderCross(center + Point(_shadowOffset, _shadowOffset), &Theme::bg1);
			renderCross(center, &Theme::fg1);

			// Text
			const float tCross = getTAt(MLX90640::frameWidth / 2, MLX90640::frameHeight / 2);

			if (!std::isnan(tCross))
				_tCross = tCross;

			constexpr static uint8_t textLength = 8;
			wchar_t text[textLength];
			std::swprintf(text, textLength, L"%.1f", _tCross);

			renderShadowedText(
				renderer,
				{
					center.getX() - _font->getWidth(text, _fontScale) / 2,
					center.getY() - crossLength / 2 - 8 - _font->getHeight(_fontScale)
				},
				text
			);
		}

		// Toolbar
		{
			constexpr static uint8_t toolbarMargin = 8;
			constexpr static uint8_t toolbarGap = 10;

			constexpr static uint8_t paletteHeight = 10;
			constexpr static uint8_t paletteTextMargin = 5;

			constexpr static uint8_t batteryWidth = 22;
			constexpr static uint8_t batteryHeight = paletteHeight;
			constexpr static uint8_t batteryTipWidth = 2;
			constexpr static uint8_t batteryTipHeight = 4;

			const int32_t paletteX = bounds.getX() + toolbarMargin;
			const int32_t paletteY = y2 - toolbarMargin - paletteHeight;
			const uint16_t paletteWidth = bounds.getWidth() - toolbarMargin - toolbarGap - batteryWidth - toolbarMargin;

			const int32_t batteryX = x2 - toolbarMargin - batteryWidth;
			const int32_t batteryY = paletteY;
			const int32_t batteryYCenter = batteryY + batteryHeight / 2;
			constexpr int32_t batteryMaxChargeWidth = batteryWidth - batteryTipWidth - 1 * 2;

			{
				float paletteIndex = 0;
				const float paletteIndexStep = static_cast<float>(palette.size() - 1) / static_cast<float>(paletteWidth - 2);

				renderer->renderFilledRectangle(
					Bounds(paletteX, paletteY, paletteWidth, paletteHeight),
					&Theme::bg1
				);

				for (uint16_t i = 0; i < paletteWidth - 2; ++i) {
					renderer->renderVerticalLine(
						Point(paletteX + 1 + i, paletteY + 1),
						paletteHeight - 2,
						&palette[static_cast<uint16_t>(paletteIndex)]
					);

					paletteIndex += paletteIndexStep;
				}
			}

			// Texts
			constexpr static uint8_t textLength = 8;
			wchar_t text[textLength];

			const auto textY = paletteY - paletteTextMargin - _font->getHeight(_fontScale);

			{
				// Left
				std::swprintf(text, textLength, L"%.1f", hMin);

				renderShadowedText(
					renderer,
					{
						paletteX,
						textY
					},
					text
				);

				// Right
				std::swprintf(text, textLength, L"%.1f", hMax);

				renderShadowedText(
					renderer,
					{
						paletteX + paletteWidth - _font->getWidth(text, _fontScale),
						textY
					},
					text
				);
			}

			// Battery
			{
				// Tip
				renderer->renderFilledRectangle(
					Bounds(batteryX, batteryYCenter - batteryTipHeight / 2, batteryTipWidth, batteryTipHeight),
					&Theme::bg1
				);

				// Body
				renderer->renderFilledRectangle(
					Bounds(batteryX + batteryTipWidth, batteryY, batteryWidth - batteryTipWidth, batteryHeight),
					&Theme::bg1
				);

				// Charge
				const int32_t batteryChargePercent = static_cast<int32_t>(th.battery.getCharge()) * 100 / 0xFF;
				const int32_t batteryChargeWidth = batteryChargePercent * batteryMaxChargeWidth / 100;

				const Color* batteryChargeColor;

				if (batteryChargePercent > 40) {
					batteryChargeColor = &Theme::green;
				}
				else if (batteryChargePercent > 20) {
					batteryChargeColor = &Theme::orange;
				}
				else {
					batteryChargeColor = &Theme::red;
				}

				renderer->renderFilledRectangle(
					Bounds(batteryX + batteryWidth - 1 - batteryChargeWidth, batteryY + 1, batteryChargeWidth, batteryHeight - 1 * 2),
					batteryChargeColor
				);
			}
		}
	}

	void ThermalView::renderShadowedText(Renderer* renderer, const Point& position, const std::wstring_view text) {
		renderer->renderString(
			position + Point(_shadowOffset, _shadowOffset),
			_font,
			&Theme::bg1,
			text,
			_fontScale
		);

		renderer->renderString(
			position,
			_font,
			&Theme::fg1,
			text,
			_fontScale
		);
	}
}
