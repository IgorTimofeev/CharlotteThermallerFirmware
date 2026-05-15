#pragma once

#include <array>

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

    		std::array<float, frameWidth * frameHeight> temperatures {};

	        void setup(i2c_master_bus_handle_t* I2CMasterBusHandle) {
        		_temperaturesMutex = xSemaphoreCreateMutex();

	            MLX90640_I2CInit(I2CMasterBusHandle, _slaveAddress, 800'000);

	            MLX90640_SetResolution(_slaveAddress, MLX90640_RESOLUTION_16_BIT);
	            MLX90640_SetRefreshRate(_slaveAddress, MLX90640_REFRESH_RATE_32_HZ);
	            MLX90640_SetChessMode(_slaveAddress);

	            MLX90640_DumpEE(_slaveAddress, _eeMLX90640);
	            MLX90640_ExtractParameters(_eeMLX90640, &_mlx90640);
	        }

    		void lockTemperatures() const {
	        	xSemaphoreTake(_temperaturesMutex, portMAX_DELAY);
	        }

    		void releaseTemperatures() const {
	        	xSemaphoreGive(_temperaturesMutex);
	        }

	        void tick() {
	        	lockTemperatures();

	            // Fetching frame
	            MLX90640_GetFrameData(_slaveAddress, _mlx90640Frame);

	            // Processing temperatures
	            _tr = MLX90640_GetTa(_mlx90640Frame, &_mlx90640) - _TA_SHIFT;
	        	MLX90640_CalculateTo(_mlx90640Frame, &_mlx90640, emissivity, _tr, temperatures.data());
	        	// MLX90640_BadPixelsCorrection(_mlx90640.brokenPixels, temperatures.data(), 1, &_mlx90640);

	     //        ESP_LOGI("MLX", "min = %f, max = %f, 766 = %f, 767 = %f", minTemperature, maxTemperature, temperatures[766], temperatures[767]);
	     //
	     //    	for (uint16_t y = 0; y < frameHeight; ++y) {
	     //    		for (uint16_t x = 0; x < frameWidth; ++x) {
						// printf("%.2f ", temperatures[y * frameWidth + x]);
	     //    		}
	     //
	     //    		printf("\n");
	     //    	}

	        	releaseTemperatures();
	        }

	    private:
	        constexpr static uint8_t _slaveAddress = 0x33;
    		constexpr static uint8_t _TA_SHIFT = 8;
	        float _tr = 0.f;
	        uint16_t _eeMLX90640[832] {};
	        uint16_t _mlx90640Frame[834] {};
	        paramsMLX90640 _mlx90640 {};

    		SemaphoreHandle_t _temperaturesMutex = nullptr;
	    };
}