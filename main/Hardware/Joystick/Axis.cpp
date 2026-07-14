#include "Axis.hpp"

#include <limits>
#include <algorithm>
#include <esp_log.h>
#include <esp_timer.h>

#include <EMAFilter.hpp>

namespace pizda {
	using namespace YOBA;

	void Axis::setup(adc_oneshot_unit_handle_t* ADCOneshotUnit, const adc_channel_t ADCChannel, const bool invertInput) {
		_ADCOneshotUnit = ADCOneshotUnit;
		_ADCChannel = ADCChannel;
		_invertInput = invertInput;

		adc_oneshot_chan_cfg_t channelConfig {};
		channelConfig.atten = ADC_ATTEN_DB_12;
		channelConfig.bitwidth = ADC_BITWIDTH_12;
		ESP_ERROR_CHECK(adc_oneshot_config_channel(*_ADCOneshotUnit, _ADCChannel, &channelConfig));

		_value = readValue();
	}

	uint16_t Axis::readValue() const {
		// Reading value
		int value;
		const auto error = adc_oneshot_read(*_ADCOneshotUnit, _ADCChannel, &value);

		if (error != ESP_OK) {
			ESP_ERROR_CHECK_WITHOUT_ABORT(error);
			return 0;
		}

		// Inverting input if required
		if (_invertInput)
			value = valueMax - value;

		return value;
	}

	void Axis::tick() {
		const auto value = readValue();

		// Applying LPF
		constexpr static uint16_t LPFFactor = std::numeric_limits<uint16_t>::max() * 20 / 100;

		_value = EMAFilter::apply(_value, value, LPFFactor);
	}

	uint16_t Axis::getValue() const {
		return _value;
	}

	ButtonAxis::ButtonAxis(const uint16_t min, const uint16_t max): _min(min), _max(max) {

	}

	void ButtonAxis::check(bool& positivePressed, bool& negativePressed, bool& razyob) {
		positivePressed = false;
		negativePressed = false;
		razyob = false;

		const auto value = std::clamp<uint16_t>(getValue(), _min, _max);
		const uint32_t threshold = static_cast<uint32_t>(_max - _min) * 20 / 100;
		const uint16_t middle = _min + (_max - _min) / 2;

		auto handle = [this, &razyob](const bool triggerCondition, bool& pressedOld, bool& pressedResult) {
			if (triggerCondition) {
				if (pressedOld) {
					if (_multiNextTimeUs > 0 && esp_timer_get_time() >= _multiNextTimeUs) {
						pressedResult = true;

						_multiNextTimeUs = esp_timer_get_time() + _multiIntervalUs;
						razyob = esp_timer_get_time() >= _multiRazyobTimeUs;
					}
				}
				else {
					pressedOld = true;

					pressedResult = true;
					_multiNextTimeUs = esp_timer_get_time() + _multiDelayUs;
					_multiRazyobTimeUs = esp_timer_get_time() + _multiRazyobDelayUs;
				}
			}
			else {
				pressedOld = false;
			}
		};

		handle(
			value >= middle + threshold,
			_posPressed,
			positivePressed
		);

		handle(
			value <= middle - threshold,
			_negPressed,
			negativePressed
		);
	}
}
