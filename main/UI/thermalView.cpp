#include "thermalView.h"

#include "thermaller.h"
#include "UI/theme.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

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

		float tMin = 99999;
		float tMax = -99999;
		float tAvg = 0;
		float t;

		std::array<float, th.MLX.temperatures.size()> tBuffer {};

		for (uint16_t i = 0; i < th.MLX.temperatures.size(); ++i) {
			t = th.MLX.temperatures[i];

			if (!std::isnan(t)) {
				tMin = std::min(tMin, t);
				tMax = std::max(tMax, t);
				tAvg += t;
			}

			tBuffer[i] = t;
		}

		tAvg /= th.MLX.temperatures.size();

		const float tMinMaxDelta = std::max<float>(tMax - tMin, 1);

		// ESP_LOGI("afa", "tmin = %f, tmax = %f, tavg = %f", tMin, tMax, tAvg);

		const uint16_t pixelSize = bounds.getWidth() / MLX90640::frameHeight;

		uint16_t tIndex = 0;

		for (uint8_t ty = 0; ty < MLX90640::frameHeight; ++ty) {
			for (uint8_t tx = 0; tx < MLX90640::frameWidth; ++tx) {
				if (std::isnan(tBuffer[tIndex]))
					tBuffer[tIndex] = tAvg;

				t = tBuffer[tIndex];
				tIndex++;

				t = std::clamp<float>(t, tMin, tMax);

				const auto ratio = (t - tMin) / tMinMaxDelta;

				auto paletteIndex = static_cast<uint16_t>(static_cast<float>(Theme::thermal.size()) * ratio);

				if (paletteIndex >= Theme::thermal.size())
					paletteIndex = Theme::thermal.size() - 1;

				int32_t pX = ty * pixelSize;
				int32_t pY = tx * pixelSize;

				renderer->renderFilledRectangle(Bounds(x2 - pixelSize - pX + 1, y2 - pixelSize - pY + 1, pixelSize, pixelSize), &Theme::thermal[paletteIndex]);
			}
		}

		// Cross
		{
			constexpr static uint8_t crossThickness = 2;
			constexpr static uint8_t crossLength = 10;

			// Horizontal
			renderer->renderFilledRectangle(
				Bounds(center.getX() - crossLength / 2, center.getY() - crossThickness / 2, crossLength, crossThickness),
				&Theme::fg1
			);

			// Vertical
			renderer->renderFilledRectangle(
				Bounds(center.getX() - crossThickness / 2, center.getY() - crossLength / 2, crossThickness, crossLength),
				&Theme::fg1
			);

			// Text
			float tCross = tBuffer[(MLX90640::frameHeight / 2) * MLX90640::frameWidth + MLX90640::frameWidth / 2];

			constexpr static uint8_t textLength = 8;
			wchar_t text[textLength];
			std::swprintf(text, textLength, L"%.2f", tCross);

			renderer->renderString(
				Point(
					center.getX() + crossLength + 10,
					center.getY() - Theme::fontBig.getHeight() / 2
				),
				&Theme::fontBig,
				&Theme::fg1,
				text
			);
		}
	}
}