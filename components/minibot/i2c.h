#pragma once
#include <memory>
#include <cstdint>

#include <driver/i2c_master.h>

namespace minibot{
typedef i2c_master_dev_handle_t I2CDevice;
typedef std::shared_ptr<I2CDevice> I2CDevicePtr;

class I2CDriver;
typedef std::shared_ptr<I2CDriver> I2CDriverPtr;

class I2CDriver{
public:
	static I2CDriverPtr makeDefault(int num, gpio_num_t sda, gpio_num_t scl);

private:
	I2CDriver(i2c_master_bus_handle_t);
	I2CDriver(const I2CDriver&)            = delete;
	I2CDriver& operator=(const I2CDriver&) = delete;

public:
	I2CDevicePtr addDevice(uint8_t addr);
	void detect();

	bool readFromMem(I2CDevicePtr dev, uint8_t reg_addr, int8_t& data, int timeout_ms=100);
	bool readFromMem(I2CDevicePtr dev, uint8_t reg_addr, int16_t& data, int timeout_ms=100);
	bool readFromMem(I2CDevicePtr dev, uint8_t reg_addr, int32_t& data, int timeout_ms=100);
	bool readFromMem(I2CDevicePtr dev, uint8_t reg_addr, uint8_t& data, int timeout_ms=100);
	bool readFromMem(I2CDevicePtr dev, uint8_t reg_addr, uint16_t& data, int timeout_ms=100);
	bool readFromMem(I2CDevicePtr dev, uint8_t reg_addr, uint32_t& data, int timeout_ms=100);
	bool readFromMem(I2CDevicePtr dev, uint8_t reg_addr, uint8_t* data, size_t len, int timeout_ms=100);

	bool writeToMem(I2CDevicePtr dev, uint8_t reg_addr, int8_t data, int timeout_ms=100);
	bool writeToMem(I2CDevicePtr dev, uint8_t reg_addr, int16_t data, int timeout_ms=100);
	bool writeToMem(I2CDevicePtr dev, uint8_t reg_addr, int32_t data, int timeout_ms=100);

	bool writeToMem(I2CDevicePtr dev, uint8_t reg_addr, uint8_t data, int timeout_ms=100);
	bool writeToMem(I2CDevicePtr dev, uint8_t reg_addr, uint16_t data, int timeout_ms=100);
	bool writeToMem(I2CDevicePtr dev, uint8_t reg_addr, uint32_t data, int timeout_ms=100);

	bool write(I2CDevicePtr dev, const uint8_t* data, size_t len, int timeout_ms=100);

private:
	i2c_master_bus_handle_t bus;
}; // End I2CDriver class
}; // End namespace