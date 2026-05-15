#pragma once

#include <esp_log.h>
#include <driver/i2c_master.h>

#include "MLX90640API.h"
#include "MLX90640I2CDriver.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace pizda {
	class MLX90640 {
		public:
			MLX90640() = default;

			void setup(i2c_master_bus_handle_t* I2CMasterBusHandle) {
				MLX90640_I2CInit(I2CMasterBusHandle, slaveAddress, 100'000);


			}

			void tick() {
				vTaskDelay(pdMS_TO_TICKS(1000));

				MLX90640_I2CGeneralReset();
				MLX90640_SetResolution(slaveAddress, 0x00);
				MLX90640_SetRefreshRate(slaveAddress, 0x03);
				MLX90640_SetChessMode(slaveAddress);

				MLX90640_DumpEE(slaveAddress, eeMLX90640);
				MLX90640_ExtractParameters(eeMLX90640, &mlx90640);

				vTaskDelay(pdMS_TO_TICKS(1000));

				ESP_LOGI("MLX", "MLX90640_GetFrameData");
				uint16_t mlx90640Frame[834] {};
				MLX90640_GetFrameData(slaveAddress, mlx90640Frame);

				ESP_LOGI("MLX", "MLX90640_GetTa");
				tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT;

				ESP_LOGI("MLX", "MLX90640_CalculateTo");
				MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, frameBuffer);

				for(int i=0;i< frameBufferLength; i++)
				{
					ESP_LOGI("MLX","%d = %f", i, frameBuffer[i]);
				}

			}

		private:
			constexpr static uint8_t TA_SHIFT = 8;
			float emissivity = 0.95;
			float tr = 0;
			unsigned char slaveAddress = 0x33;
			uint16_t eeMLX90640[832] {};
			paramsMLX90640 mlx90640 {};

			constexpr static uint8_t frameWidth = 32;
			constexpr static uint8_t frameHeight = 24;
			constexpr static uint16_t frameBufferLength = frameWidth * frameHeight;
			float frameBuffer[frameBufferLength] {};
	};
}