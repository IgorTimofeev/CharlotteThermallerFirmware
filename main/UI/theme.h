#pragma once

#include <YOBA/main.h>
#include <YOBA/UI.h>
#include <YOBA/resources/fonts/PIXY10Font.h>
#include <YOBA/resources/fonts/Vaticanus8Font.h>
#include <YOBA/resources/fonts/unscii16.h>

namespace pizda {
	using namespace YOBA;

	class Theme {
		public:
			constexpr static Bit8PaletteColor bg1 { 0 };
			constexpr static Bit8PaletteColor bg2 { 1 };
			constexpr static Bit8PaletteColor bg3 { 2 };
			constexpr static Bit8PaletteColor bg4 { 3 };
			constexpr static Bit8PaletteColor bg5 { 4 };
			constexpr static Bit8PaletteColor bg6 { 5 };
			constexpr static Bit8PaletteColor bg7 { 6 };

			constexpr static Bit8PaletteColor fg1 { 7 };
			constexpr static Bit8PaletteColor fg2 { 8 };
			constexpr static Bit8PaletteColor fg3 { 9 };
			constexpr static Bit8PaletteColor fg4 { 10 };
			constexpr static Bit8PaletteColor fg5 { 11 };
			constexpr static Bit8PaletteColor fg6 { 12 };
			constexpr static Bit8PaletteColor fg7 { 13 };

			constexpr static Bit8PaletteColor accent1 { 14 };
			constexpr static Bit8PaletteColor accent2 { 15 };

			constexpr static Bit8PaletteColor good1 { 16 };
			constexpr static Bit8PaletteColor good2 { 17 };

			constexpr static Bit8PaletteColor bad1 { 18 };
			constexpr static Bit8PaletteColor bad2 { 19 };
			constexpr static Bit8PaletteColor bad3 { 20 };

			constexpr static Bit8PaletteColor red { 21 };
			constexpr static Bit8PaletteColor green1 { 22 };
			constexpr static Bit8PaletteColor green2 { 23 };
			constexpr static Bit8PaletteColor blue { 24 };
			constexpr static Bit8PaletteColor magenta1 { 25 };
			constexpr static Bit8PaletteColor magenta2 { 26 };
			constexpr static Bit8PaletteColor ocean { 27 };

			constexpr static Bit8PaletteColor yellow { 28 };
			constexpr static Bit8PaletteColor ground1 { 29 };
			constexpr static Bit8PaletteColor ground2 { 30 };
			constexpr static Bit8PaletteColor sky1 { 31 };
			constexpr static Bit8PaletteColor sky2 { 32 };

			constexpr static PIXY10Font fontNormal {};
			constexpr static Vaticanus8Font fontSmall {};

			constexpr static uint16_t elementHeight = 27;
			constexpr static uint16_t cornerRadius = 3;
			constexpr static uint16_t verticalGap = 9;
			constexpr static uint16_t horizontalGap = 7;

			static void setup(Bit8PaletteRenderer* renderer);
	};
}