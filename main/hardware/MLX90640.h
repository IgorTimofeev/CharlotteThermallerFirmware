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
				MLX90640I2CDriverInit(I2CMasterBusHandle, slaveAddress, 400'000);
				status = MLX90640_DumpEE (slaveAddress, eeMLX90640);
				status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
			}

			void tick() {
				return;
				// default mode is chess mode, only every second pixel will be updated
				//2 subframes needed for full image update
				//default sampling on sensor is 2Hz.

				MLX90640_GetSubFrameData (slaveAddress, mlx90640Frame);
				tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT;
				MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, frameBuffer);

				MLX90640_GetSubFrameData (slaveAddress, mlx90640Frame);
				tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT;
				MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, frameBuffer);

				for(int p=0;p< frameBufferLength; p++)
				{
					ESP_LOGI("MLX","T[%i]=%.2f", p, frameBuffer[p]);
				}

				vTaskDelay( xDelay );
			}

		private:
			constexpr static uint8_t TA_SHIFT = 8;
			float emissivity = 0.95;
			float tr = 0;
			unsigned char slaveAddress = 0x33;
			uint16_t eeMLX90640[832] {};
			uint16_t mlx90640Frame[834] {};
			paramsMLX90640 mlx90640 {};

			constexpr static uint8_t frameWidth = 32;
			constexpr static uint8_t frameHeight = 24;
			constexpr static uint16_t frameBufferLength = frameWidth * frameHeight;
			float frameBuffer[frameBufferLength] {};

			int status = 0;
			const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;
	};
}