#include "thermalView.h"

#include <ranges>

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

		std::array<float, th.MLX.frame.size()> frame {};

		xSemaphoreTake(th.MLX.frameMutex, portMAX_DELAY);
		std::ranges::copy(th.MLX.frame, frame.begin());
		xSemaphoreGive(th.MLX.frameMutex);
		
		const auto x2 = bounds.getX2();
		const auto y2 = bounds.getY2();
		const auto center = bounds.getCenter();

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

			const bool autoHistorgam = true;

			if (autoHistorgam) {
				// Applying LPF
				if (false && _histogramLPFProcessed) {
					constexpr static float LPFFactor = 0.2f;

					_hMin = LowPassFilter::apply(_hMin, tMin, LPFFactor);
					_hMax = LowPassFilter::apply(_hMax, tMax, LPFFactor);
				}
				else {
					_hMin = tMin;
					_hMax = tMax;

					_histogramLPFProcessed = true;
				}
			}
			// Manual histogram
			else {
				_hMin = 26;
				_hMax = 32;
			}
		}

		const float tMinMaxDelta = std::max<float>(_hMax - _hMin, 1);

		// ESP_LOGI("afa", "tmin = %f, tmax = %f, tavg = %f", tMin, tMax, tAvg);

		const uint16_t pSize = bounds.getWidth() / MLX90640::frameHeight;

		const auto getTAt = [&frame](const uint8_t x, const uint8_t y) -> float {
			return frame[y * MLX90640::frameWidth + x];
		};

		const auto getColor = [tMinMaxDelta, this, tAvg](float t) -> const RGB565Color* {
			if (std::isnan(t))
				return &Theme::bg1;

			t = std::clamp(std::isnan(t) ? tAvg : t, _hMin, _hMax);

			const auto ratio = (t - _hMin) / tMinMaxDelta;
			auto index = static_cast<uint16_t>(std::round(static_cast<float>(_palette->size()) * ratio));

			if (index >= _palette->size())
				index = _palette->size() - 1;

			return &(*_palette)[index];
		};

		bool interpolation = false;

		if (interpolation) {
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
			const float paletteIndexStep = static_cast<float>(_palette->size() - 1) / static_cast<float>(paletteWidth - 2);

			renderer->renderFilledRectangle(
				Bounds(paletteX, paletteY, paletteWidth, paletteHeight),
				&Theme::bg1
			);

			for (uint16_t i = 0; i < paletteWidth - 2; ++i) {
				renderer->renderVerticalLine(
					Point(paletteX + 1 + i, paletteY + 1),
					paletteHeight - 2,
					&(*_palette)[static_cast<uint16_t>(paletteIndex)]
				);

				paletteIndex += paletteIndexStep;
			}

			// Texts
			constexpr static uint8_t textLength = 8;
			wchar_t text[textLength];

			const auto textY = paletteY - paletteTextMargin - _font->getHeight();

			// Left
			std::swprintf(text, textLength, L"%.1f", _hMin);

			renderShadowedText(
				renderer,
				{
					paletteX,
					textY
				},
				text
			);

			// Right
			std::swprintf(text, textLength, L"%.1f", _hMax);

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
