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

					constexpr static gpio_num_t SCL = GPIO_NUM_46;
					constexpr static gpio_num_t SDA = GPIO_NUM_45;
			};

			class SPI {
				public:
					constexpr static spi_host_device_t hostDevice = SPI2_HOST;
					constexpr static gpio_num_t SCK = GPIO_NUM_15;
					constexpr static gpio_num_t MOSI = GPIO_NUM_16;
					constexpr static gpio_num_t MISO = GPIO_NUM_21;
			};

			class screen {
				public:
					constexpr static gpio_num_t SS = GPIO_NUM_17;
					constexpr static gpio_num_t DC = GPIO_NUM_18;
					constexpr static gpio_num_t RST = GPIO_NUM_48;

					constexpr static uint32_t SPIFrequency = 60'000'000;
			};

			class battery {
				public:
					constexpr static gpio_num_t transistorPin = GPIO_NUM_6;

					constexpr static adc_unit_t ADCUnit = ADC_UNIT_1;
					constexpr static adc_channel_t ADCChannel = ADC_CHANNEL_0;

					constexpr static uint32_t voltageMin = 3'000;
					constexpr static uint32_t voltageMax = 4'100;

					constexpr static uint32_t dividerResistanceR1 = 100'000;
					constexpr static uint32_t dividerResistanceR2 = 330'000;
			};

			class buzzer {
				public:
					constexpr static gpio_num_t gpio = GPIO_NUM_5;
					constexpr static ledc_channel_t channel = LEDC_CHANNEL_0;
			};

			class joystick {
				public:
					class axisX {
						public:
							constexpr static adc_unit_t unit = ADC_UNIT_1;
							constexpr static adc_channel_t channel = ADC_CHANNEL_2;
							constexpr static bool invertInput = true;
					};

					class axisY {
						public:
							constexpr static adc_unit_t unit = ADC_UNIT_1;
							constexpr static adc_channel_t channel = ADC_CHANNEL_1;
							constexpr static bool invertInput = true;
					};

					constexpr static gpio_num_t button = GPIO_NUM_4;
			};
	};
}
