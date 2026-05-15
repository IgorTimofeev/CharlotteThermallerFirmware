#include "MLX90640I2CDriver.h"

#include <bit>

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

i2c_master_dev_handle_t MLX90640I2CDriverDevice {};

void MLX90640_I2CInit(i2c_master_bus_handle_t* bus, uint8_t address, uint32_t clockSpeedHz)
{
    i2c_device_config_t deviceConfig {};
    deviceConfig.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    deviceConfig.device_address = address;
    deviceConfig.scl_speed_hz = clockSpeedHz;

    const auto state = i2c_master_bus_add_device(*bus, &deviceConfig, &MLX90640I2CDriverDevice);
    ESP_ERROR_CHECK(state);
}

int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{
    const uint8_t command[] {
        static_cast<uint8_t>(startAddress >> 8),
        static_cast<uint8_t>(startAddress & 0x00FF)
    };

    uint8_t buffer[1664];

    const auto state = i2c_master_transmit_receive(
        MLX90640I2CDriverDevice,
        command,
        2,
        buffer,
        nMemAddressRead * 2,
        500
    );

    ESP_ERROR_CHECK(state);

	const auto bufferUint16 = reinterpret_cast<uint16_t*>(buffer);

    for (uint16_t wordIndex = 0; wordIndex < nMemAddressRead; wordIndex++)
        data[wordIndex] = std::byteswap(bufferUint16[wordIndex]);

    return state;
}

int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)
{
    const uint8_t command[] {
        static_cast<uint8_t>(writeAddress >> 8),
        static_cast<uint8_t>(writeAddress & 0x00FF),
        static_cast<uint8_t>(data >> 8),
        static_cast<uint8_t>(data & 0x00FF),
    };

    const auto state = i2c_master_transmit(
        MLX90640I2CDriverDevice,
        command,
        4,
        500
    );

    ESP_ERROR_CHECK(state);

    return state;
}

int MLX90640_I2CGeneralReset()
{
    constexpr uint8_t data[] {
        0x00,
        0x06
    };

    const auto state = i2c_master_transmit(
       MLX90640I2CDriverDevice,
       data,
       2,
       500
    );

    ESP_ERROR_CHECK(state);

    return state;
}

