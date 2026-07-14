#pragma once

#include "Resources/Sounds/BootSound.hpp"
#include "Resources/Sounds/FeedbackSound.hpp"

namespace pizda::resources {
	class sounds {
		public:
			constexpr static BootSound boot {};
			constexpr static FeedbackSound feedback {};
	};
}