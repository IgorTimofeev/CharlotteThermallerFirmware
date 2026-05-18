#pragma once

#include <cstdint>

#include <NVSSettings.h>

#include "UI/thermalView.h"

namespace pizda {
	using namespace YOBA;

	class Settings : public NVSSettings {
		public:
			ThermalPalette thermalPalette = ThermalPalette::govno;

		protected:
			const char* getNamespace() override {
				return "ms";
			}

			void onRead(const NVSStream& stream) override {
				thermalPalette = stream.readEnum<ThermalPalette>(_thermalPalette, ThermalPalette::govno);
			}

			void onWrite(const NVSStream& stream) override {
				stream.writeEnum<ThermalPalette>(_thermalPalette, thermalPalette);
			}

		private:
			constexpr static auto _thermalPalette = "tp";
	};
}
