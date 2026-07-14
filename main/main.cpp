#include "Thermaller.hpp"

extern "C" {
	void app_main(void) {
		pizda::Thermaller::getInstance().start();
	}
}