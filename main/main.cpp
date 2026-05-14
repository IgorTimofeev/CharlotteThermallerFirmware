#include "thermaller.h"

extern "C" {
	void app_main(void) {
		pizda::Thermaller::getInstance().start();
	}
}