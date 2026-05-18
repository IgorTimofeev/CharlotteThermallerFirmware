#pragma once

#include <algorithm>
#include <esp_adc/adc_oneshot.h>

namespace pizda {
	class Axis {
		public:
			Axis() = default;
			virtual ~Axis() = default;

			constexpr static uint8_t valueLengthBits = 12;
			constexpr static uint16_t valueMax = (1 << valueLengthBits) - 1;

			void setup(adc_oneshot_unit_handle_t* ADCOneshotUnit, adc_channel_t ADCChannel, bool invertInput);
			void tick();

			uint16_t getValue() const;

		private:
			adc_oneshot_unit_handle_t* _ADCOneshotUnit = nullptr;
			adc_channel_t _ADCChannel {};
			bool _invertInput = false;
			uint16_t _value = 0;

			uint16_t readValue() const;
	};

	class ButtonAxis : public Axis {
		public:
			ButtonAxis(const uint16_t min, const uint16_t max);

			void tick(bool& positivePressed, bool& negativePressed);

		private:
			uint16_t _min;
			uint16_t _max;

			bool _posPressed = false;
			bool _negPressed = false;
	};
}