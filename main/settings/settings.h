#pragma once

#include <cstdint>

#include <NVSSettings.h>

#include "UI/thermalView.h"

namespace pizda {
	using namespace YOBA;

	enum class SettingsSampleRate : uint8_t {
		hz0_5,
		hz1,
		hz2,
		hz4,
		hz8,
		hz16,
		hz32,
		hz64,

		maxValue = hz64
	};

	class Settings : public NVSSettings {
		public:
			SettingsSampleRate sampleRate = SettingsSampleRate::hz32;

			bool reflectedTemperatureAuto = false;
			int16_t reflectedTemperatureValue = 0;

			uint8_t emissivityPercent = 0;

			ThermalPalette thermalPalette = ThermalPalette::hunting;

			bool rangeAuto = true;
			int16_t rangeMin = 0;
			int16_t rangeMax = 0;

			bool interpolation = false;

		protected:
			const char* getNamespace() override {
				return "ms";
			}

			void onRead(const NVSStream& stream) override {
				sampleRate = stream.readEnum<SettingsSampleRate>(_sampleRate, SettingsSampleRate::hz32);

				reflectedTemperatureAuto = stream.readBool(_reflectedTemperatureAuto, true);
				reflectedTemperatureValue = stream.readInt16(_reflectedTemperatureValue, 26);

				emissivityPercent = stream.readUint8(_emissivityPercent, 95);

				thermalPalette = stream.readEnum<ThermalPalette>(_thermalPalette, ThermalPalette::hunting);

				rangeAuto = stream.readBool(_rangeAuto, true);
				rangeMin = stream.readInt16(_rangeMin, 22);
				rangeMax = stream.readInt16(_rangeMax, 37);

				interpolation = stream.readBool(_interpolation, false);
			}

			void onWrite(const NVSStream& stream) override {
				stream.writeEnum<SettingsSampleRate>(_sampleRate, sampleRate);

				stream.writeBool(_reflectedTemperatureAuto, reflectedTemperatureAuto);
				stream.writeInt16(_reflectedTemperatureValue, reflectedTemperatureValue);

				stream.writeUint8(_emissivityPercent, emissivityPercent);

				stream.writeEnum<ThermalPalette>(_thermalPalette, thermalPalette);

				stream.writeBool(_rangeAuto, rangeAuto);
				stream.writeInt16(_rangeMin, rangeMin);
				stream.writeInt16(_rangeMax, rangeMax);

				stream.writeBool(_interpolation, interpolation);
			}

		private:
			constexpr static auto _sampleRate = "sr";

			constexpr static auto _reflectedTemperatureAuto = "fa";
			constexpr static auto _reflectedTemperatureValue = "fv";

			constexpr static auto _emissivityPercent = "ep";
			constexpr static auto _temperatureShift = "ts";

			constexpr static auto _thermalPalette = "tp";

			constexpr static auto _rangeAuto = "ra";
			constexpr static auto _rangeMin = "rm";
			constexpr static auto _rangeMax = "rx";

			constexpr static auto _interpolation = "in";
	};
}
