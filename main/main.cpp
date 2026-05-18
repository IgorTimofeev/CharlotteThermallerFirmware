#include "thermaller.h"

extern "C" {
	void app_main(void) {
		ESP_LOGI("afea", "pizda???");

		pizda::Thermaller::getInstance().start();
	}
}