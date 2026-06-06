#include "hardware/MLX90640/MLX90640.h"

#include <array>

#include <esp_log.h>
#include <thermaller.h>
#include <driver/i2c_master.h>

#include "MLX90640API.h"
#include "MLX90640I2CDriver.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace pizda {
	void MLX90640::setup(i2c_master_bus_handle_t* I2CMasterBusHandle) {

		frameMutex = xSemaphoreCreateMutex();

		MLX90640_I2CInit(I2CMasterBusHandle, _slaveAddress, 800'000);
		// vTaskDelay(pdMS_TO_TICKS(1000));

		// MLX90640_I2CGeneralReset();
		// vTaskDelay(pdMS_TO_TICKS(1000));


		// // Should be 0x1901 at 0x240c
		// // See https://www.sekorm.com/news/93963947.html
		// uint16_t tmp = 0;
		// MLX90640_I2CRead(_slaveAddress, 0x240c, 1, &tmp);
		// ESP_LOGI(_logTag, "buffer: %d", tmp);
		//
		// // Restoring original value
		// vTaskDelay(pdMS_TO_TICKS(1000));
		// MLX90640_I2CWrite(_slaveAddress, 0x240c, 0x1901);
		// vTaskDelay(pdMS_TO_TICKS(1000));

		// printEEPROMData();

		if constexpr (config::MLX::useDumpedEEPROMData) {
			MLX90640_ExtractParameters(const_cast<uint16_t*>(config::MLX::dumpedEEPROMData), &_params);
		}
		else {
			uint16_t _ee[832] {};
			MLX90640_DumpEE(_slaveAddress, _ee);
			MLX90640_ExtractParameters(_ee, &_params);
		}

		MLX90640_SetResolution(_slaveAddress, MLX90640_RESOLUTION_16_BIT);
		MLX90640_SetChessMode(_slaveAddress);

		setRefreshRateFromSettings();
	}

	void MLX90640::setRefreshRateFromSettings() {
		const auto& th = Thermaller::getInstance();

		uint8_t refreshRateValue;

		switch (th.settings.refreshRate) {
			case SettingsRefreshRate::hz0_5:
				refreshRateValue = MLX90640_REFRESH_RATE_0_5_HZ;
				break;
			case SettingsRefreshRate::hz1:
				refreshRateValue = MLX90640_REFRESH_RATE_1_HZ;
				break;
			case SettingsRefreshRate::hz2:
				refreshRateValue = MLX90640_REFRESH_RATE_2_HZ;
				break;
			case SettingsRefreshRate::hz4:
				refreshRateValue = MLX90640_REFRESH_RATE_4_HZ;
				break;
			case SettingsRefreshRate::hz8:
				refreshRateValue = MLX90640_REFRESH_RATE_8_HZ;
				break;
			case SettingsRefreshRate::hz16:
				refreshRateValue = MLX90640_REFRESH_RATE_16_HZ;
				break;
			case SettingsRefreshRate::hz32:
				refreshRateValue = MLX90640_REFRESH_RATE_32_HZ;
				break;
			default:
				refreshRateValue = MLX90640_REFRESH_RATE_64_HZ;
				break;
		}

		MLX90640_SetRefreshRate(_slaveAddress, refreshRateValue);
	}

	void MLX90640::tick() {
		const auto& th = Thermaller::getInstance();

		// Fetching raw frame data
		MLX90640_GetFrameData(_slaveAddress, _frameData);

		// Computing reflected temperature. Ideally, it should be determined manually by known surroundings
		// temperature like walls, floor, etc. But for approximate estimation we could use
		// internal sensor temperature (which will be higher, ofc) and subtract ~5-12 degrees from it
		float tr;

		if (th.settings.reflectedTemperatureAuto) {
			tr = MLX90640_GetTa(_frameData, &_params) - _TA_SHIFT;
		}
		else {
			tr = th.settings.reflectedTemperatureValue;
		}

		// Computing temperatures from raw frame data
		xSemaphoreTake(frameMutex, portMAX_DELAY);

		MLX90640_CalculateTo(
			_frameData,
			&_params,
			static_cast<float>(th.settings.emissivityPercent) / 100.f,
			tr,
			frame.data()
		);

		xSemaphoreGive(frameMutex);

		// ESP_LOGI(_logTag, "766 = %f, 767 = %f", frame[766], frame[767]);
	}

	void MLX90640::printEEPROMData() {
		uint16_t _ee[832] {};
		MLX90640_DumpEE(_slaveAddress, _ee);

		ESP_LOGI(_logTag, "EEPROM dump started");

		printf("\n");

		// --
		// --
		// --

		for (size_t i = 0; i < 832; ++i) {
			printf("0x%04X, ", _ee[i]);

			if ((i + 1) % 16 == 0)
				printf("\n");
		}

		printf("\n\n");

		ESP_LOGI(_logTag, "EEPROM dump finished");

		while (true) {
			vTaskDelay(pdMS_TO_TICKS(1'000));
		}
	}
}
