#pragma once

#include <cstdint>

#include <NVSSettings.h>

#include "UI/thermalView.h"

namespace pizda {
	using namespace YOBA;

	class Settings : public NVSSettings {
		public:
			ThermalPalette thermalPalette = ThermalPalette::hunting;
			uint8_t emissivityPercent = 95;
			bool interpolation = false;

			bool rangeAuto = true;
			int16_t rangeMin = 0;
			int16_t rangeMax = 0;

		protected:
			const char* getNamespace() override {
				return "ms";
			}

			void onRead(const NVSStream& stream) override {
				thermalPalette = stream.readEnum<ThermalPalette>(_thermalPalette, ThermalPalette::hunting);
				emissivityPercent = stream.readUint8(_emissivityPercent, 95);
				interpolation = stream.readBool(_interpolation, false);

				rangeAuto = stream.readBool(_rangeAuto, true);
				rangeMin = stream.readInt16(_rangeMin, 22);
				rangeMax = stream.readInt16(_rangeMax, 37);
			}

			void onWrite(const NVSStream& stream) override {
				stream.writeEnum<ThermalPalette>(_thermalPalette, thermalPalette);
				stream.writeUint8(_emissivityPercent, emissivityPercent);
				stream.writeBool(_interpolation, interpolation);

				stream.writeBool(_rangeAuto, rangeAuto);
				stream.writeInt16(_rangeMin, rangeMin);
				stream.writeInt16(_rangeMax, rangeMax);
			}

		private:
			constexpr static auto _thermalPalette = "tp";
			constexpr static auto _emissivityPercent = "ep";
			constexpr static auto _interpolation = "in";

			constexpr static auto _rangeAuto = "ra";
			constexpr static auto _rangeMin = "rm";
			constexpr static auto _rangeMax = "rx";
	};
}
