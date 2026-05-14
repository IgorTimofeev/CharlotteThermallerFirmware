#include "theme.h"

namespace pizda {
	void Theme::setup(Bit8PaletteRenderer* renderer) {
		renderer->setPaletteColors({
			// Background
			RGB888Color(0x000000),
			RGB888Color(0x0C0C0C),
			RGB888Color(0x1C1C1C),
			RGB888Color(0x2C2C2C),
			RGB888Color(0x3C3C3C),
			RGB888Color(0x4C4C4C),
			RGB888Color(0x5C5C5C),

			// Foreground
			RGB888Color(0xF0F0F0),
			RGB888Color(0xCCCCCC),
			RGB888Color(0x999999),
			RGB888Color(0x888888),
			RGB888Color(0x777777),
			RGB888Color(0x666666),
			RGB888Color(0x555555),

			// Accent
			RGB888Color(0xffd200),
			RGB888Color(0x997e53),

			// Good
			RGB888Color(0x1db153),
			RGB888Color(0x28DA69),

			// Bad
			RGB888Color(0xf24949),
			RGB888Color(0x962828),
			RGB888Color(0x4b1313),

			// Red
			RGB888Color(0xff0000),
			// Green
			RGB888Color(0x00FF00),
			RGB888Color(0x007F00),
			// Blue
			RGB888Color(0x0000FF),
			// Magenta (R.I.P. PURPLE)
			RGB888Color(0xFF00FF),
			RGB888Color(0xAA00AA),
			// Ocean
			RGB888Color(0x00FFFF),

			// Yellow
			RGB888Color(0xffd200),
			// Ground
			RGB888Color(0x97b838),
			RGB888Color(0xdef2a2),
			// Sky
			RGB888Color(0x317fcb),
			RGB888Color(0xaed1f2),
		});
	}
}