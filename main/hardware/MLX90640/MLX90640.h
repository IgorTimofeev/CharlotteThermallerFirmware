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

    		std::array<float, frameWidth * frameHeight> frame {};

    		SemaphoreHandle_t frameMutex = nullptr;

	        void setup(i2c_master_bus_handle_t* I2CMasterBusHandle) {
        		frameMutex = xSemaphoreCreateMutex();

	            MLX90640_I2CInit(I2CMasterBusHandle, _slaveAddress, 800'000);

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

	        	uint16_t _ee[832] {};
	        	MLX90640_DumpEE(_slaveAddress, _ee);
	        	MLX90640_ExtractParameters(_ee, &_params);

	            MLX90640_SetResolution(_slaveAddress, MLX90640_RESOLUTION_16_BIT);
	            MLX90640_SetRefreshRate(_slaveAddress, MLX90640_REFRESH_RATE_32_HZ);
	            MLX90640_SetChessMode(_slaveAddress);
	        }

	        void tick() {

	            // Fetching frame
	            MLX90640_GetFrameData(_slaveAddress, _frameData);

	            // Processing temperatures
	            const auto tr = MLX90640_GetTa(_frameData, &_params) - _TA_SHIFT;

	        	xSemaphoreTake(frameMutex, portMAX_DELAY);
	        	MLX90640_CalculateTo(_frameData, &_params, emissivity, tr, frame.data());
	        	xSemaphoreGive(frameMutex);

				// ESP_LOGI(_logTag, "766 = %f, 767 = %f", frame[766], frame[767]);
	     //
	     //    	for (uint16_t y = 0; y < frameHeight; ++y) {
	     //    		for (uint16_t x = 0; x < frameWidth; ++x) {
						// printf("%.2f ", temperatures[y * frameWidth + x]);
	     //    		}
	     //
	     //    		printf("\n");
	     //    	}

	        }

	    private:
    		constexpr static auto _logTag = "MLX";
	        constexpr static uint8_t _slaveAddress = 0x33;
    		constexpr static uint8_t _TA_SHIFT = 8;
	        uint16_t _frameData[834] {};
	        paramsMLX90640 _params {};
	    };
}