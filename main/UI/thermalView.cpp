#include <ranges>
#include <span>

#include "Thermaller.hpp"
#include "UI/ThermalView.hpp"
#include "UI/Theme.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <EMAFilter.hpp>
#include "Resources/Sounds.hpp"

#include "hardware/MLX90640/MLX90640.hpp"
#include "Hardware/Joystick/Joystick.hpp"

namespace pizda {
	void ThermalView::onTick() {
		invalidate();
	}

	void ThermalView::onEvent(Event* event) {
		Control::onEvent(event);

		const auto joystickEvent = event->as<JoystickEvent>();

		if (!joystickEvent)
			return;

		if (joystickEvent->type == JoystickEventType::press) {
			auto& th = Thermaller::getInstance();

			th.setRoute(MenuRoute::main);

			th.audioPlayer.play(&resources::sounds::feedback);
		}

		event->setHandled(true);
	}

	void ThermalView::onRender(Renderer* renderer, const Rectangle& bounds) {
		auto& th = Thermaller::getInstance();

		// Keeping this shit for later use
		const auto x2 = bounds.getX2();
		const auto y2 = bounds.getY2();
		const auto center = bounds.getCenter();

		// Fetching latest MLX frame. Since rendering might take a while, it would be nice to copy it
		std::array<int16_t, th.MLX.frame.size()> frame {};

		// Fetching histogram data
		int16_t histogramMin = 0;
		int16_t histogramMax = 0;
		int32_t histogramAvg = 0;

		{
			int16_t minTemp = std::numeric_limits<int16_t>::max();
			int16_t maxTemp = std::numeric_limits<int16_t>::min();

			// Copying measured temperatures
			xSemaphoreTake(th.MLX.frameMutex, portMAX_DELAY);

			for (size_t i = 0; i < frame.size(); ++i) {
				const auto sourceTemp = th.MLX.frame[i];

				// NaN values is a common problem with MLX modules - it usually means that current pixel is broken
				// We will mark such pixels as shitty ones and replace with average values later
				if (std::isnan(sourceTemp)) {
					frame[i] = std::numeric_limits<int16_t>::min();
				}
				else {
					// Copying source float temperature and converting it to integer value with factor of 10
					// So 36.6 deg Celsius will become 366 deca... Celsius? Whatever
					auto frameTemp = static_cast<int16_t>(sourceTemp * 10.f);

					frame[i] = frameTemp;

					// Computing min / max / avg temperatures
					if (frameTemp < minTemp)
						minTemp = frameTemp;

					if (frameTemp > maxTemp)
						maxTemp = frameTemp;

					histogramAvg += frameTemp;
				}
			}

			xSemaphoreGive(th.MLX.frameMutex);

			// Computing average temperature
			histogramAvg /= frame.size();

			// Replacing all shitty pixels with computed average value
			for (auto& i : frame) {
				if (i == std::numeric_limits<int16_t>::min()) {
					i = static_cast<int16_t>(histogramAvg);
				}
			}

			// Applying histogram range based on settings
			if (th.settings.rangeAuto) {
				histogramMin = minTemp;
				histogramMax = maxTemp;
			}
			else {
				histogramMin = th.settings.rangeMin;
				histogramMax = th.settings.rangeMax;
			}
		}

		const auto hMaxMinDelta = histogramMax - histogramMin;

		// Determining which palette to use
		std::span<const RGB565Color> palette;

		switch (th.settings.thermalPalette) {
			case ThermalPalette::hunting: palette = Theme::thermalPaletteHunting; break;
			case ThermalPalette::ironbow: palette =Theme::thermalPaletteIronbow; break;
			case ThermalPalette::arctic: palette = Theme::thermalPaletteArctic ; break;
			case ThermalPalette::whiteHot: palette = Theme::thermalPaletteWhiteHot; break;
			case ThermalPalette::blackHot: palette = Theme::thermalPaletteBlackHot; break;
		}

		// Rendering frame
		{
			constexpr static uint16_t framePixelSize = 10;

			const auto getTemperatureColor = [hMaxMinDelta, histogramMin, histogramMax, palette](int16_t t) -> const RGB565Color* {
				t = std::clamp(t, histogramMin, histogramMax);

				auto paletteIndex = static_cast<uint16_t>((t - histogramMin) * palette.size() / hMaxMinDelta);

				if (paletteIndex >= palette.size())
					paletteIndex = palette.size() - 1;

				return &palette[paletteIndex];
			};

			// Interpolation is SOOO FREAKING SLOW, even with direct pixel buffer access
			// Totally not recommended, -nan/10 experience
			if (th.settings.interpolation) {
				// Pre-calculated LUT with rounded weights for interpolation using factor of 256
				// Later values will be shifted using >> 8 for FAST AS FUCK rendering
				//
				// Since we have 10 screen pixels in frame pixel, we should use something like
				// float ratio = subPixelX / 10.f;
				//
				// But we can replace this shit with LUT of weights and use integers for faster rendering
				// The final interpolated temperature values will be shifted using >> 8 to get rid of 256
				static const uint16_t weightsP[framePixelSize] = { 0, 26, 51, 77, 102, 128, 154, 179, 205, 230 }; // ratio * 256
				static const uint16_t weightsN[framePixelSize] = { 256, 230, 205, 179, 154, 128, 102, 77, 51, 26 }; // (1 - ratio) * 256

				// Copying entire frame to get rid of range checks in loop
				int16_t interpolationFrame[MLX90640::frameHeight + 1][MLX90640::frameWidth + 1];

				for (uint8_t y = 0; y < MLX90640::frameHeight; ++y) {
				    for (uint8_t x = 0; x < MLX90640::frameWidth; ++x) {
				        interpolationFrame[y][x] = frame[y * MLX90640::frameWidth + x];
				    }

				    // Duplicating last column
				    interpolationFrame[y][MLX90640::frameWidth] = interpolationFrame[y][MLX90640::frameWidth - 1];
				}

				// Duplicating last row
				for (uint8_t x = 0; x <= MLX90640::frameWidth; ++x) {
				    interpolationFrame[MLX90640::frameHeight][x] = interpolationFrame[MLX90640::frameHeight - 1][x];
				}

				// Interpolation itself
				for (uint8_t frameY = 0; frameY < MLX90640::frameHeight; ++frameY) {
					const int32_t screenX11 = x2 - frameY * framePixelSize;

				    for (uint8_t subPixelY = 0; subPixelY < framePixelSize; ++subPixelY) {
				    	// Taking weights for current row
				        const auto weightYN = weightsN[subPixelY];
				        const auto weightYP = weightsP[subPixelY];

				        for (uint8_t frameX = 0; frameX < MLX90640::frameWidth; ++frameX) {
				        	const int32_t screenY11 = y2 - frameX * framePixelSize;

				        	// Taking 4 adjacent points
				            const int32_t t11 = interpolationFrame[frameY][frameX];
				            const int32_t t21 = interpolationFrame[frameY][frameX + 1];
				            const int32_t t12 = interpolationFrame[frameY + 1][frameX];
				            const int32_t t22 = interpolationFrame[frameY + 1][frameX + 1];

							// const float tIValue =
							//       (1 - tIRatioX) * (1 - tIRatioY) * t11
							//       + tIRatioX * (1 - tIRatioY) * t21
							//       + (1 - tIRatioX) * tIRatioY * t12
							//       + tIRatioX * tIRatioY * t22;

				        	// Pre-calculating vertical values for this pair of columns
				        	// Shifting >> 8 removes factor of 256 from weights LUT
				        	const int32_t col1 = (t11 * weightYN + t12 * weightYP) >> 8;
				        	const int32_t col2 = (t21 * weightYN + t22 * weightYP) >> 8;

				            for (uint8_t subPixelX = 0; subPixelX < framePixelSize; ++subPixelX) {
				            	const auto interpolatedTemperature = static_cast<int16_t>((col1 * weightsN[subPixelX] + col2 * weightsP[subPixelX]) >> 8);

				            	renderer->putPixel(
				            		Point(screenX11 - subPixelY, screenY11 - subPixelX),
				            		getTemperatureColor(interpolatedTemperature)
				            	);
				            }
				        }
				    }
				}
			}
			// Blazingly 🔥 fast 🚀 as diarrhea, HOLD THIS
			else {
				Rectangle pBounds { 0, 0, framePixelSize, framePixelSize};

				for (uint8_t frameY = 0; frameY < MLX90640::frameHeight; ++frameY) {
					pBounds.setX(x2 - framePixelSize - frameY * framePixelSize);

					for (uint8_t frameX = 0; frameX < MLX90640::frameWidth; ++frameX) {
						pBounds.setY(y2 - framePixelSize - frameX * framePixelSize);

						renderer->fillRectangle(
							pBounds,
							getTemperatureColor(frame[frameY * MLX90640::frameWidth + frameX])
						);
					}
				}
			}
		}

		// Rendering cross & frame center temperature
		{
			constexpr static uint8_t crossThickness = 2;
			constexpr static uint8_t crossLength = crossThickness * 4;

			const auto renderCross = [renderer](const Point& position, const Color* color) {
				renderer->fillRectangle(
					{
						position.getX() - crossLength / 2,
						position.getY() - crossThickness / 2,
						crossLength,
						crossThickness
					},
					color
				);

				renderer->fillRectangle(
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
			constexpr static uint8_t textLength = 8;
			char text[textLength];

			std::snprintf(
				text,
				textLength,
				"%.1f",
				static_cast<float>(frame[MLX90640::frameHeight / 2 * MLX90640::frameWidth + MLX90640::frameWidth / 2])
					/ 10.f
			);

			renderShadowedText(
				renderer,
				{
					center.getX() - _font->getWidth(_fontScale, text) / 2,
					center.getY() - crossLength / 2 - 8 - _font->getLineHeight(_fontScale)
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

				renderer->fillRectangle(
					Rectangle(histogramX, histogramY, histogramWidth, histogramHeight),
					&Theme::bg1
				);

				for (uint16_t i = 0; i < histogramWidth - 2; ++i) {
					renderer->strokeVerticalLine(
						Point(histogramX + 1 + i, histogramY + 1),
						histogramHeight - 2,
						&palette[static_cast<uint16_t>(paletteIndex)]
					);

					paletteIndex += paletteIndexStep;
				}

				// Texts
				constexpr static uint8_t textLength = 8;
				char text[textLength];

				const auto textY = histogramY - histogramTextMargin - _font->getLineHeight(_fontScale);

				// Left
				std::snprintf(text, textLength, "%.1f", static_cast<float>(histogramMin) / 10.f);

				renderShadowedText(
					renderer,
					{
						histogramX,
						textY
					},
					text
				);

				// Right
				std::snprintf(text, textLength, "%.1f", static_cast<float>(histogramMax) / 10.f);

				renderShadowedText(
					renderer,
					{
						histogramX + histogramWidth - _font->getWidth(_fontScale, text),
						textY
					},
					text
				);
			}

			// Rendering battery
			{
				// Tip
				renderer->fillRectangle(
					Rectangle(batteryX, batteryYCenter - batteryTipHeight / 2, batteryTipWidth, batteryTipHeight),
					&Theme::bg1
				);

				// Body
				renderer->fillRectangle(
					Rectangle(batteryX + batteryTipWidth, batteryY, batteryWidth - batteryTipWidth, batteryHeight),
					&Theme::bg1
				);

				// Charge
				const auto batteryCharge = th.battery.getCharge16();
				const uint16_t batteryChargePercent = static_cast<uint16_t>(batteryCharge) * 100 / 0xFFFF;
				const int32_t batteryChargeWidth = divideRounding<int32_t>(batteryCharge * batteryMaxChargeWidth, 0xFFFF);

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

				renderer->fillRectangle(
					Rectangle(batteryX + batteryWidth - 1 - batteryChargeWidth, batteryY + 1, batteryChargeWidth, batteryHeight - 1 * 2),
					batteryChargeColor
				);
			}
		}
	}

	void ThermalView::renderShadowedText(Renderer* renderer, const Point& position, const std::string_view text) {
		renderer->putText(
			position + Point(_shadowOffset, _shadowOffset),
			_font,
			_fontScale,
			&Theme::bg1,
			text
		);

		renderer->putText(
			position,
			_font,
			_fontScale,
			&Theme::fg1,
			text
		);
	}
}
