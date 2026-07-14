#pragma once

#include <array>

#include <esp_log.h>
#include <driver/i2c_master.h>

#include "Hardware/MLX90640/MLX90640API.hpp"
#include "Hardware/MLX90640/MLX90640I2CDriver.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace pizda {
    class MLX90640 {
	    public:
	        MLX90640() = default;

	        constexpr static uint8_t frameWidth = 32;
	        constexpr static uint8_t frameHeight = 24;

    		std::array<float, frameWidth * frameHeight> frame {};

    		SemaphoreHandle_t frameMutex = nullptr;

	        void setup(i2c_master_bus_handle_t* I2CMasterBusHandle);
	        void setRefreshRateFromSettings();

	        void tick();

        private:
    		constexpr static auto _logTag = "MLX";
	        constexpr static uint8_t _slaveAddress = 0x33;
    		constexpr static uint8_t _TA_SHIFT = 8;
	        uint16_t _frameData[834] {};
	        paramsMLX90640 _params {};

    		void printEEPROMData();
    };
}