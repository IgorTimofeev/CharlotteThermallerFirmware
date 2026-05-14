#pragma once

#include <cstdint>

#include <driver/spi_master.h>
#include <esp_adc/adc_oneshot.h>

#include <SX1262.h>

#include "types/generic.h"

namespace pizda {
	class config {
		public:
			class SPI {
				public:
					constexpr static spi_host_device_t device = SPI2_HOST;
					constexpr static gpio_num_t MOSI = GPIO_NUM_23;
					constexpr static gpio_num_t MISO = GPIO_NUM_19;
					constexpr static gpio_num_t SCK = GPIO_NUM_18;
			};

			class I2C {
				public:
					constexpr static gpio_num_t SDA = GPIO_NUM_21;
					constexpr static gpio_num_t SCL = GPIO_NUM_22;
			};

			class OTA {
				public:
					constexpr static const char* url = "https://github.com/IgorTimofeev/CharlotteDownloads/raw/refs/heads/master/Firmware/Remote.bin";
			};

			class screen {
				public:
					class touch {
						public:
							constexpr static gpio_num_t RST = GPIO_NUM_NC;
							constexpr static gpio_num_t INTR = GPIO_NUM_5;
					};

					constexpr static gpio_num_t SS = GPIO_NUM_17;
					constexpr static gpio_num_t DC = GPIO_NUM_16;
					constexpr static gpio_num_t RST = GPIO_NUM_NC;
					constexpr static uint32_t frequency = 60'000'000;
			};
	
			class XCVR {
				public:
					constexpr static gpio_num_t SS = GPIO_NUM_14;
					constexpr static gpio_num_t busy = GPIO_NUM_12;
					constexpr static gpio_num_t DIO1 = GPIO_NUM_13;
					constexpr static gpio_num_t RST = GPIO_NUM_NC;

					// SX1262 supports up to 16 MHz, but with long wires (10+ cm) there will be troubles, so
					constexpr static uint32_t SPIFrequencyHz = 10'000'000;

					// Default values, can be changed and stored in NVS
					constexpr static TransceiverCommunicationSettings communicationSettings {
						915'000'000,
						SX1262::LoRaBandwidth::bw500_0,
						7,
						SX1262::LoRaCodingRate::cr4_5,
						0x34,
						8,

						60,
						22
					};
			};
	
			class axes {
				public:
					constexpr static uint8_t tickRateHz = 30;

					class leverLeft {
						public:
							constexpr static adc_unit_t unit = ADC_UNIT_1;
							constexpr static adc_channel_t channel = ADC_CHANNEL_0;
							constexpr static bool invertInput = false;
					};

					class leverRight {
						public:
							constexpr static adc_unit_t unit = ADC_UNIT_1;
							constexpr static adc_channel_t channel = ADC_CHANNEL_3;
							constexpr static bool invertInput = false;
					};

					class joystickHorizontal {
						public:
							constexpr static adc_unit_t unit = ADC_UNIT_1;
							constexpr static adc_channel_t channel = ADC_CHANNEL_7;
							constexpr static bool invertInput = true;
					};

					class joystickVertical {
						public:
							constexpr static adc_unit_t unit = ADC_UNIT_1;
							constexpr static adc_channel_t channel = ADC_CHANNEL_5;
							constexpr static bool invertInput = false;
					};

					class ring {
						public:
							constexpr static adc_unit_t unit = ADC_UNIT_1;
							constexpr static adc_channel_t channel = ADC_CHANNEL_6;
							constexpr static bool invertInput = true;
					};
			};
	
			class encoder {
				public:
					constexpr static gpio_num_t a = GPIO_NUM_26;
					constexpr static gpio_num_t b = GPIO_NUM_25;
					constexpr static gpio_num_t sw = GPIO_NUM_27;
			};
	
			/**
			Some thoughts about measuring voltage & charge in percents using ADC:
	
			1) Safe voltage range for Li-ion 18650 battery is [2.5; 4.2]V, and for 2x batteries
			in series it escalates to [5.0; 8.4]V. But let's give it some safety margins like
			[6.0; 8.4]V, because of tons of trash batteries on market
	
			2) In theory ADC should receive up to 3.3V from GPIO, but Espressif docs says that ADC
			configured with 12 dB attenuation can accurately measure only [0.15; 2.45]V on ESP32
			See: https://docs.espressif.com/projects/esp-idf/en/release-v4.3/esp32/api-reference/peripherals/adc.html
	
			Based on this shit & resistors I have, the voltage divider will be 1M / 330K,
			giving final input range of [1.488; 2.084]V
			*/
			class battery {
				public:
					class remote {
						public:
							constexpr static adc_unit_t unit = ADC_UNIT_1;
							constexpr static adc_channel_t channel = ADC_CHANNEL_4;

							constexpr static uint32_t voltageMin = 2 * 3'000;
							constexpr static uint32_t voltageMax = 2 * 4'200;

							constexpr static uint32_t voltageDividerR1 = 1000'000;
							constexpr static uint32_t voltageDividerR2 = 330'000;
					};

					class aircraft {
						public:
							constexpr static uint32_t voltageMin = 4 * 3'000;
							constexpr static uint32_t voltageMax = 4 * 4'200;
					};
			};
	
			class buzzer {
				public:
					constexpr static gpio_num_t gpio = GPIO_NUM_4;
					constexpr static ledc_channel_t channel = LEDC_CHANNEL_0;
			};

			class camera {
				public:
					constexpr static int16_t servoAngularRangeDeg = 180;
					constexpr static int16_t servoMaxDeg = servoAngularRangeDeg / 2;
					constexpr static int16_t servoMinDeg = -servoMaxDeg;

					constexpr static int16_t pitchMinDeg = servoMinDeg;
					constexpr static int16_t pitchMaxDeg = 10;

					constexpr static int16_t yawMinDeg = servoMinDeg;
					constexpr static int16_t yawMaxDeg = servoMaxDeg;

					constexpr static int16_t pitchCorrectionYawThresholdMinDeg = 40;
					constexpr static int16_t pitchCorrectionYawThresholdMaxDeg = std::min<int16_t>(yawMaxDeg, 90);
					constexpr static int16_t pitchCorrectionByMaxDeg = 30;

					static void clamp(int16_t& pitch, int16_t& yaw) {
						pitch = std::clamp<int16_t>(
							std::clamp<int16_t>(
								pitch,
								pitchMinDeg,
								pitchMaxDeg
							),
							servoMinDeg,
							servoMaxDeg
						);

						yaw = std::clamp<int16_t>(
							std::clamp<int16_t>(
								yaw,
								yawMinDeg,
								yawMaxDeg
							),
							servoMinDeg,
							servoMaxDeg
						);
					}

					static void correctPitchPitchForYaw(int16_t& pitch, int16_t yaw) {
						// Yaw abs
						if (yaw < 0)
							yaw = -yaw;

						// Nothing to do
						if (yaw <= pitchCorrectionYawThresholdMinDeg)
							return;

						// Changing pitch by difference between yaw and min/max threshold
						pitch +=
							static_cast<int16_t>(pitchCorrectionByMaxDeg)
							* (yaw - pitchCorrectionYawThresholdMinDeg)
							/ (pitchCorrectionYawThresholdMaxDeg - pitchCorrectionYawThresholdMinDeg);
					}
			};

			class application {
				public:
					// 30 FPS are barely achievable on SPI displays, BUT
					constexpr static uint8_t interfaceTickRateHz = 60;
					constexpr static uint8_t peripheralTickRateHz = 20;
			};
	};
}
