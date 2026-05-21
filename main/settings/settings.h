#pragma once

#include <cstdint>

#include <NVSSettings.h>

#include "UI/thermalView.h"

namespace pizda {
	using namespace YOBA;

	enum class SettingsRefreshRate : uint8_t {
		hz1,
		hz2,
		hz4,
		hz8,
		hz16,
		hz32,

		maxValue = hz32
	};

	class Settings : public NVSSettings {
		public:
			SettingsRefreshRate refreshRate = SettingsRefreshRate::hz32;

			ThermalPalette thermalPalette = ThermalPalette::hunting;
			uint8_t emissivityPercent = 95;
			uint8_t ambientTemperatureShift = 8;

			bool interpolation = false;
			bool chessMode = true;

			bool rangeAuto = true;
			int16_t rangeMin = 0;
			int16_t rangeMax = 0;

		protected:
			const char* getNamespace() override {
				return "ms";
			}

			void onRead(const NVSStream& stream) override {
				refreshRate = stream.readEnum<SettingsRefreshRate>(_refreshRate, SettingsRefreshRate::hz32);

				thermalPalette = stream.readEnum<ThermalPalette>(_thermalPalette, ThermalPalette::hunting);
				emissivityPercent = stream.readUint8(_emissivityPercent, 95);
				ambientTemperatureShift = stream.readUint8(_ambientTemperatureShift, 8);
				interpolation = stream.readBool(_interpolation, false);
				chessMode = stream.readBool(_chessMode, true);

				rangeAuto = stream.readBool(_rangeAuto, true);
				rangeMin = stream.readInt16(_rangeMin, 22);
				rangeMax = stream.readInt16(_rangeMax, 37);
			}

			void onWrite(const NVSStream& stream) override {
				stream.writeEnum<SettingsRefreshRate>(_refreshRate, refreshRate);

				stream.writeEnum<ThermalPalette>(_thermalPalette, thermalPalette);
				stream.writeUint8(_emissivityPercent, emissivityPercent);
				stream.writeUint8(_ambientTemperatureShift, ambientTemperatureShift);
				stream.writeBool(_interpolation, interpolation);
				stream.writeBool(_chessMode, chessMode);

				stream.writeBool(_rangeAuto, rangeAuto);
				stream.writeInt16(_rangeMin, rangeMin);
				stream.writeInt16(_rangeMax, rangeMax);
			}

		private:
			constexpr static auto _refreshRate = "rr";
			constexpr static auto _thermalPalette = "tp";
			constexpr static auto _emissivityPercent = "ep";
			constexpr static auto _ambientTemperatureShift = "as";
			constexpr static auto _interpolation = "in";
			constexpr static auto _chessMode = "ch";

			constexpr static auto _rangeAuto = "ra";
			constexpr static auto _rangeMin = "rm";
			constexpr static auto _rangeMax = "rx";
	};
}
