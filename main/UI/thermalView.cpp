#include "thermalView.h"

#include <ranges>
#include <span>

#include "thermaller.h"
#include "UI/theme.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lowPassFilter.h>
#include <resources/sounds.h>

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

			th.setRoute(MenuRoute::main);

			th.audioPlayer.play(&resources::sounds::feedback);
		}

		event->setHandled(true);
	}

	void ThermalView::onRender(Renderer* renderer, const Bounds& bounds) {
		auto& th = Thermaller::getInstance();

		// Keeping this shit for later use
		const auto x2 = bounds.getX2();
		const auto y2 = bounds.getY2();
		const auto center = bounds.getCenter();

		// Fetching latest MLX frame. Since rendering might take a while, it would be nice to copy it
		std::array<float, th.MLX.frame.size()> frame {};

		xSemaphoreTake(th.MLX.frameMutex, portMAX_DELAY);
		std::ranges::copy(th.MLX.frame, frame.begin());
		xSemaphoreGive(th.MLX.frameMutex);

		// Fetching histogram data
		float hMin = 0;
		float hMax = 0;
		float hAvg = 0;

		{
			float tMin = 99999;
			float tMax = -99999;

			for (uint16_t i = 0; i < frame.size(); ++i) {
				if (std::isnan(frame[i]))
					continue;

				tMin = std::min(tMin, frame[i]);
				tMax = std::max(tMax, frame[i]);
				hAvg += frame[i];
			}

			hAvg /= frame.size();

			if (th.settings.rangeAuto) {
				hMin = tMin;
				hMax = tMax;
			}
			else {
				hMin = th.settings.rangeMin;
				hMax = th.settings.rangeMax;
			}
		}

		const float hMinMaxDelta = std::max<float>(hMax - hMin, 1);

		const auto getFrameTemperature = [&frame](const uint8_t x, const uint8_t y) -> float {
			return frame[y * MLX90640::frameWidth + x];
		};

		// Determining which palette to use
		std::span<const RGB565Color> palette;

		switch (th.settings.thermalPalette) {
			case ThermalPalette::hunting: palette = Theme::thermalPaletteHunting; break;
			case ThermalPalette::ironbow: palette =Theme::thermalPaletteIronbow; break;
			case ThermalPalette::rainbow: palette = Theme::thermalPaletteRainbow; break;
			case ThermalPalette::arctic: palette = Theme::thermalPaletteArctic ; break;
			case ThermalPalette::whiteHot: palette = Theme::thermalPaletteWhiteHot; break;
			case ThermalPalette::blackHot: palette = Theme::thermalPaletteBlackHot; break;
		}

		// Rendering frame
		{
			const uint16_t pSize = bounds.getWidth() / MLX90640::frameHeight;

			const auto getTemperatureColor = [hMinMaxDelta, hAvg, hMin, hMax, palette](float t) -> const RGB565Color* {
				if (std::isnan(t))
					return &Theme::bg1;

				t = std::clamp(std::isnan(t) ? hAvg : t, hMin, hMax);

				const auto ratio = (t - hMin) / hMinMaxDelta;
				auto index = static_cast<uint16_t>(std::round(static_cast<float>(palette.size()) * ratio));

				if (index >= palette.size())
					index = palette.size() - 1;

				return &palette[index];
			};

			// Interpolation is SOOO FREAKING SLOW, even with direct pixel buffer access
			// Totally not recommended, -nan/10 experience
			if (th.settings.interpolation) {
				for (uint8_t tY = 0; tY < MLX90640::frameHeight; ++tY) {
					const int32_t pX = x2 - tY * pSize;

					for (uint8_t tX = 0; tX < MLX90640::frameWidth; ++tX) {
						const int32_t pY = y2 - tX * pSize;

						const uint8_t tX1 = std::min<uint8_t>(tX + 1, MLX90640::frameWidth - 1);
						const uint8_t tY1 = std::min<uint8_t>(tY + 1, MLX90640::frameHeight - 1);

						const float t11 = getFrameTemperature(tX, tY);
						const float t12 = getFrameTemperature(tX, tY1);
						const float t21 = getFrameTemperature(tX1, tY);
						const float t22 = getFrameTemperature(tX1, tY1);

						for (uint8_t tIY = 0; tIY < pSize; ++tIY) {
							const float tIRatioX = static_cast<float>(tIY) / static_cast<float>(pSize);

							for (uint8_t tIX = 0; tIX < pSize; ++tIX) {
								const float tIRatioY = static_cast<float>(tIX) / static_cast<float>(pSize);

								const float tIValue =
									(1 - tIRatioX) * (1 - tIRatioY) * t11
									+ tIRatioX * (1 - tIRatioY) * t21
									+ (1 - tIRatioX) * tIRatioY * t12
									+ tIRatioX * tIRatioY * t22;

								renderer->renderPixel(Point(pX - tIX, pY - tIY), getTemperatureColor(tIValue));
							}
						}
					}
				}
			}
			// Blazingly 🔥 fast 🚀 as diarrhea, HOLD THIS
			else {
				Bounds pBounds { 0, 0, pSize, pSize};

				for (uint8_t tY = 0; tY < MLX90640::frameHeight; ++tY) {
					pBounds.setX(x2 - pSize - tY * pSize);

					for (uint8_t tX = 0; tX < MLX90640::frameWidth; ++tX) {
						pBounds.setY(y2 - pSize - tX * pSize);

						renderer->renderFilledRectangle(pBounds, getTemperatureColor(getFrameTemperature(tX, tY)));
					}
				}
			}
		}

		// Rendering cross & frame center temperature
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
			const float tCross = getFrameTemperature(MLX90640::frameWidth / 2, MLX90640::frameHeight / 2);

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

		// Rendering histogram & battery indicator
		{
			constexpr static uint8_t toolbarMargin = 8;
			constexpr static uint8_t toolbarGap = 10;

			constexpr static uint8_t histogramHeight = 10;
			constexpr static uint8_t histogramTextMargin = 5;

			constexpr static uint8_t batteryWidth = 22;
			constexpr static uint8_t batteryHeight = histogramHeight;
			constexpr static uint8_t batteryTipWidth = 2;
			constexpr static uint8_t batteryTipHeight = 4;

			const int32_t histogramX = bounds.getX() + toolbarMargin;
			const int32_t histogramY = y2 - toolbarMargin - histogramHeight;
			const uint16_t histogramWidth = bounds.getWidth() - toolbarMargin - toolbarGap - batteryWidth - toolbarMargin;

			const int32_t batteryX = x2 - toolbarMargin - batteryWidth;
			const int32_t batteryY = histogramY;
			const int32_t batteryYCenter = batteryY + batteryHeight / 2;
			constexpr int32_t batteryMaxChargeWidth = batteryWidth - batteryTipWidth - 1 * 2;

			// Rendering histogram
			{
				float paletteIndex = 0;
				const float paletteIndexStep = static_cast<float>(palette.size() - 1) / static_cast<float>(histogramWidth - 2);

				renderer->renderFilledRectangle(
					Bounds(histogramX, histogramY, histogramWidth, histogramHeight),
					&Theme::bg1
				);

				for (uint16_t i = 0; i < histogramWidth - 2; ++i) {
					renderer->renderVerticalLine(
						Point(histogramX + 1 + i, histogramY + 1),
						histogramHeight - 2,
						&palette[static_cast<uint16_t>(paletteIndex)]
					);

					paletteIndex += paletteIndexStep;
				}

				// Texts
				constexpr static uint8_t textLength = 8;
				wchar_t text[textLength];

				const auto textY = histogramY - histogramTextMargin - _font->getHeight(_fontScale);

				// Left
				std::swprintf(text, textLength, L"%.1f", hMin);

				renderShadowedText(
					renderer,
					{
						histogramX,
						textY
					},
					text
				);

				// Right
				std::swprintf(text, textLength, L"%.1f", hMax);

				renderShadowedText(
					renderer,
					{
						histogramX + histogramWidth - _font->getWidth(text, _fontScale),
						textY
					},
					text
				);
			}

			// Rendering battery
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
				const auto batteryCharge = th.battery.getCharge();
				const uint16_t batteryChargePercent = static_cast<uint16_t>(batteryCharge) * 100 / 0xFF;
				const int32_t batteryChargeWidth = divideRounding<int32_t>(batteryCharge * batteryMaxChargeWidth, 0xFF);

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
