#pragma once

#include "resources/sounds/bootSound.h"
#include "resources/sounds/feedbackSound.h"

namespace pizda::resources {
	class sounds {
		public:
			constexpr static BootSound boot {};
			constexpr static FeedbackSound feedback {};
	};
}