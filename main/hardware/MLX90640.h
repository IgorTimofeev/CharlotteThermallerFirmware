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

	        float emissivity = 0.95f;
	        constexpr static uint8_t frameWidth = 32;
	        constexpr static uint8_t frameHeight = 24;
	        constexpr static uint16_t temperaturesLength = frameWidth * frameHeight;
	        float temperatures[temperaturesLength] {};
    		SemaphoreHandle_t temperaturesMutex = nullptr;

	        void setup(i2c_master_bus_handle_t* I2CMasterBusHandle) {
        		temperaturesMutex = xSemaphoreCreateMutex();

	            MLX90640_I2CInit(I2CMasterBusHandle, slaveAddress, 800'000);

	            MLX90640_SetResolution(slaveAddress, MLX90640_RESOLUTION_16_BIT);
	            MLX90640_SetRefreshRate(slaveAddress, MLX90640_REFRESH_RATE_32_HZ);
	            MLX90640_SetChessMode(slaveAddress);

	            MLX90640_DumpEE(slaveAddress, eeMLX90640);
	            MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
	        }

	        void tick() {
        		xSemaphoreTake(temperaturesMutex, portMAX_DELAY);

	            // Fetching frame
	            MLX90640_GetFrameData(slaveAddress, mlx90640Frame);

	            // Processing temperatures
	            tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT;
	        	MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, temperatures);

        		xSemaphoreGive(temperaturesMutex);

	            ESP_LOGI("MLX", "766 = %f, 767 = %f", temperatures[766], temperatures[767]);
	        }

	    private:
	        constexpr static uint8_t TA_SHIFT = 8;
	        float tr = 0.f;
	        unsigned char slaveAddress = 0x33;
	        uint16_t eeMLX90640[832] {};
	        uint16_t mlx90640Frame[834] {};
	        paramsMLX90640 mlx90640 {};
	    };
}