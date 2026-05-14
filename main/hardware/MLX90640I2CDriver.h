#ifndef _I2C_DRIVER_
#define _I2C_DRIVER_


#define SDA_PIN 21
#define SCL_PIN 22
#define I2C_BUS_FREQUENCY_HZ 1000000
#define ACK_CHECK_EN 0x1
#define ACK_CHECK_DIS 0x0
#define ACK_VAL 0x0
#define NACK_VAL 0x1
#define I2C_MASTER_TIMEOUT_MS 1000

#include <stdint.h>
#include <driver/i2c_master.h>
void MLX90640I2CDriverInit(i2c_master_bus_handle_t* bus, uint8_t address, uint32_t clockSpeedHz);
int MLX90640I2CDriverRead(uint8_t slaveAddr,uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data);
int MLX90640I2CDriverWrite(uint8_t slaveAddr,uint16_t writeAddress, uint16_t data);
#endif