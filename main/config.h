#pragma once

#include <cstdint>

#include <driver/spi_master.h>
#include <driver/ledc.h>
#include <driver/gpio.h>
#include <esp_adc/adc_oneshot.h>

namespace pizda {
	class config {
		public:
			class I2C {
				public:
					constexpr static i2c_port_t port = I2C_NUM_0;

					constexpr static gpio_num_t SCL = GPIO_NUM_4;
					constexpr static gpio_num_t SDA = GPIO_NUM_5;
			};

			class SPI {
				public:
					constexpr static spi_host_device_t hostDevice = SPI2_HOST;
					constexpr static gpio_num_t SCK = GPIO_NUM_6;
					constexpr static gpio_num_t MOSI = GPIO_NUM_7;
					constexpr static gpio_num_t MISO = GPIO_NUM_15;
			};

			class screen {
				public:
					constexpr static gpio_num_t SS = GPIO_NUM_16;
					constexpr static gpio_num_t DC = GPIO_NUM_17;
					constexpr static gpio_num_t RST = GPIO_NUM_18;

					constexpr static uint32_t SPIFrequency = 60'000'000;
			};

			// class battery {
			// 	public:
			// 		class remote {
			// 			public:
			// 				constexpr static adc_unit_t unit = ADC_UNIT_1;
			// 				constexpr static adc_channel_t channel = ADC_CHANNEL_4;
			//
			// 				constexpr static uint32_t voltageMin = 2 * 3'000;
			// 				constexpr static uint32_t voltageMax = 2 * 4'200;
			//
			// 				constexpr static uint32_t voltageDividerR1 = 1000'000;
			// 				constexpr static uint32_t voltageDividerR2 = 330'000;
			// 		};
			//
			// 		class aircraft {
			// 			public:
			// 				constexpr static uint32_t voltageMin = 4 * 3'000;
			// 				constexpr static uint32_t voltageMax = 4 * 4'200;
			// 		};
			// };
			//
			// class buzzer {
			// 	public:
			// 		constexpr static gpio_num_t gpio = GPIO_NUM_4;
			// 		constexpr static ledc_channel_t channel = LEDC_CHANNEL_0;
			// };
	};
}
