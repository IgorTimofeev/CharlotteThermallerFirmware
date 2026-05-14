#include "MLX90640I2CDriver.h"
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

i2c_master_dev_handle_t MLX90640I2CDriverDevice {};
constexpr uint16_t MLX90640I2CDriverBufferLength = 2048;
uint8_t MLX90640I2CDriverBuffer[MLX90640I2CDriverBufferLength];

void MLX90640I2CDriverInit(i2c_master_bus_handle_t* bus, uint8_t address, uint32_t clockSpeedHz)
{
    i2c_device_config_t deviceConfig {};
    deviceConfig.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    deviceConfig.device_address = address;
    deviceConfig.scl_speed_hz = clockSpeedHz;

    const auto state = i2c_master_bus_add_device(*bus, &deviceConfig, &MLX90640I2CDriverDevice);
    ESP_ERROR_CHECK(state);
}

int MLX90640I2CDriverRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{
    const uint8_t command[] {
        static_cast<uint8_t>(startAddress >> 8),
        static_cast<uint8_t>(startAddress & 0x00FF)
    };

    const auto state = i2c_master_transmit_receive(
        MLX90640I2CDriverDevice,
        command,
        2,
        MLX90640I2CDriverBuffer,
        nMemAddressRead,
        I2C_MASTER_TIMEOUT_MS
    );

    ESP_ERROR_CHECK(state);

    int cnt = 0;
    int i = 0;
    uint16_t *p;

    p = data;

    for(cnt=0; cnt < nMemAddressRead; cnt++)  {
        i = cnt << 1;
        *p++ = static_cast<uint16_t>(MLX90640I2CDriverBuffer[i])*256 + static_cast<uint16_t>(MLX90640I2CDriverBuffer[i + 1]);
    }

    return state;
} 

int MLX90640I2CDriverWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)
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
        I2C_MASTER_TIMEOUT_MS
    );

    ESP_ERROR_CHECK(state);

    static uint16_t dataCheck;

    MLX90640I2CDriverRead(slaveAddr,writeAddress,1, &dataCheck);

    if ( dataCheck != data)
    {
        return -2;
    }   
    
    return 0;
}
