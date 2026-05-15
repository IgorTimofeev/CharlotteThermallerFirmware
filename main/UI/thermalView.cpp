#include "thermalView.h"

#include "thermaller.h"
#include "UI/theme.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lowPassFilter.h>

#include "hardware/MLX90640.h"

namespace pizda {
	ThermalView::ThermalView() {

	}

	void ThermalView::onTick() {
		invalidate();
	}

	void ThermalView::onRender(Renderer* renderer, const Bounds& bounds) {
		auto& th = Thermaller::getInstance();

		const auto x2 = bounds.getX2();
		const auto y2 = bounds.getY2();
		const auto center = bounds.getCenter();

		// Min / max / avg
		{
			float tMin = 99999;
			float tMax = -99999;
			float tAvg = 0;

			for (uint16_t i = 0; i < th.MLX.frame.size(); ++i) {
				float t = th.MLX.frame[i];

				if (!std::isnan(t)) {
					tMin = std::min(tMin, t);
					tMax = std::max(tMax, t);
					tAvg += t;
				}
			}

			tAvg /= th.MLX.frame.size();

			// Applying LPF
			if (true) {
				constexpr static float LPFFactor = 0.1f;

				_tMin = LowPassFilter::apply(_tMin, tMin, LPFFactor);
				_tMax = LowPassFilter::apply(_tMax, tMax, LPFFactor);
				_tAvg = LowPassFilter::apply(_tAvg, tAvg, LPFFactor);
			}
			else {
				_tMin = tMin;
				_tMax = tMax;
				_tAvg = tAvg;
			}
		}

		const float tMinMaxDelta = std::max<float>(_tMax - _tMin, 1);

		// ESP_LOGI("afa", "tmin = %f, tmax = %f, tavg = %f", tMin, tMax, tAvg);

		const uint16_t pSize = bounds.getWidth() / MLX90640::frameHeight;

		const auto getTAt = [&th, this](const uint8_t x, const uint8_t y) -> float {
			const uint16_t index = y * MLX90640::frameWidth + x;

			return std::isnan(th.MLX.frame[index]) ? _tAvg : th.MLX.frame[index];
		};

		const auto tToColor = [tMinMaxDelta, this](const float t) -> const RGB565Color* {
			const auto ratio = (t - _tMin) / tMinMaxDelta;
			auto index = static_cast<uint16_t>(static_cast<float>(Theme::thermal.size()) * ratio);

			if (index >= Theme::thermal.size())
				index = Theme::thermal.size() - 1;

			return &Theme::thermal[index];
		};

		if (false) {
			for (uint8_t tY = 0; tY < MLX90640::frameHeight - 1; ++tY) {
				const int32_t pX = x2 - tY * pSize;

				for (uint8_t tX = 0; tX < MLX90640::frameWidth - 1; ++tX) {
					const int32_t pY = y2 - tX * pSize;

					const float t11 = getTAt(tX, tY);
					const float t12 = getTAt(tX, tY + 1);
					const float t21 = getTAt(tX + 1, tY);
					const float t22 = getTAt(tX + 1, tY + 1);

					for (uint8_t iY = 0; iY < pSize; ++iY) {
						const float ratioX = static_cast<float>(iY) / static_cast<float>(pSize);

						for (uint8_t iX = 0; iX < pSize; ++iX) {
							const float ratioY = static_cast<float>(iX) / static_cast<float>(pSize);

							const float tI =
								(1 - ratioX) * (1 - ratioY) * t11
								+ ratioX * (1 - ratioY) * t21
								+ (1 - ratioX) * ratioY * t12
								+ ratioX * ratioY * t22;

							renderer->renderPixel(Point(pX - iX, pY - iY), tToColor(tI));
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

					renderer->renderFilledRectangle(
						pBounds,
						tToColor(getTAt(tX, tY))
					);
				}
			}
		}

		// Cross
		{
			constexpr static uint8_t crossThickness = 2;
			constexpr static uint8_t crossLength = crossThickness * 3;

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
			_tCross = LowPassFilter::apply(_tCross, tCross, 0.1f);

			constexpr static uint8_t textLength = 8;
			wchar_t text[textLength];
			std::swprintf(text, textLength, L"%.1f", _tCross);

			renderShadowedText(
				renderer,
				{
					center.getX() - _font->getWidth(text) / 2,
					center.getY() - crossLength / 2 - 8 - _font->getHeight()
				},
				text
			);
		}

		// Palette
		{
			constexpr static uint8_t paletteHeight = 10;
			constexpr static uint8_t paletteMargin = 8;
			constexpr static uint8_t paletteTextMargin = 5;

			const int32_t paletteX = bounds.getX() + paletteMargin;
			const int32_t paletteY = y2 - paletteMargin - paletteHeight + 1;
			const uint16_t paletteWidth = bounds.getWidth() - paletteMargin * 2;

			float paletteIndex = 0;
			const float paletteIndexStep = static_cast<float>(Theme::thermal.size() - 1) / static_cast<float>(paletteWidth - 2);

			renderer->renderFilledRectangle(
				Bounds(paletteX, paletteY, paletteWidth, paletteHeight),
				&Theme::bg1
			);

			for (uint16_t i = 0; i < paletteWidth - 2; ++i) {
				renderer->renderVerticalLine(
					Point(paletteX + 1 + i, paletteY + 1),
					paletteHeight - 2,
					&Theme::thermal[static_cast<uint16_t>(paletteIndex)]
				);

				paletteIndex += paletteIndexStep;
			}

			// Texts
			constexpr static uint8_t textLength = 8;
			wchar_t text[textLength];

			const auto textY = paletteY - paletteTextMargin - _font->getHeight();

			// Left
			std::swprintf(text, textLength, L"%.1f", _tMin);

			renderShadowedText(
				renderer,
				{
					paletteX,
					textY
				},
				text
			);

			// Right
			std::swprintf(text, textLength, L"%.1f", _tMax);

			renderShadowedText(
				renderer,
				{
					paletteX + paletteWidth - _font->getWidth(text),
					textY
				},
				text
			);
		}
	}

	void ThermalView::renderShadowedText(Renderer* renderer, const Point& position, const std::wstring_view text) {
		renderer->renderString(
			position + Point(_shadowOffset, _shadowOffset),
			_font,
			&Theme::bg1,
			text
		);

		renderer->renderString(
			position,
			_font,
			&Theme::fg1,
			text
		);
	}
}
