#include "i2c.h"

using namespace minibot;

I2CDriver::I2CDriver(i2c_master_bus_handle_t bh): bus(bh){}

I2CDriverPtr I2CDriver::makeDefault(int port, gpio_num_t sda, gpio_num_t scl){
	i2c_master_bus_handle_t bus;
	i2c_master_bus_config_t bus_config = {
		.i2c_port   = port,
		.sda_io_num = sda,
		.scl_io_num = scl,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.intr_priority     = 0,
		.trans_queue_depth = 0,
		.flags             = {
			.enable_internal_pullup = true,
			.allow_pd               = false,
		},
	};
	if( i2c_new_master_bus(&bus_config, &bus) != ESP_OK)
		return nullptr;
	return std::shared_ptr<I2CDriver>(new I2CDriver(bus));
}

I2CDevicePtr I2CDriver::addDevice(uint8_t addr){
	I2CDevicePtr dev_handle = std::shared_ptr<I2CDevice>(new I2CDevice());
	i2c_device_config_t dev_config = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address  = addr,
		.scl_speed_hz    = CONFIG_I2C_FREQUENCY,
		.scl_wait_us     = 0,
		.flags           = {},
	};
	if( i2c_master_bus_add_device(bus, &dev_config, dev_handle.get()) != ESP_OK)
		return nullptr;
	return dev_handle;
}

bool I2CDriver::readFromMem(I2CDevicePtr dev, uint8_t reg_addr, int8_t& data, int timeout){
	return i2c_master_transmit_receive(*dev, &reg_addr, 1, (uint8_t*)&data, 1, timeout) == ESP_OK;
}


bool I2CDriver::readFromMem(I2CDevicePtr dev, uint8_t reg_addr, int16_t& data, int timeout){
	bool ret = i2c_master_transmit_receive(*dev, &reg_addr, 1, (uint8_t*)&data, 2, timeout) == ESP_OK;
	data = __builtin_bswap16(data);
	return ret;
}


bool I2CDriver::readFromMem(I2CDevicePtr dev, uint8_t reg_addr, int32_t& data, int timeout){
	bool ret = i2c_master_transmit_receive(*dev, &reg_addr, 1, (uint8_t*)&data, 4, timeout) == ESP_OK;
	data = __builtin_bswap32(data);
	return ret;
}


bool I2CDriver::readFromMem(I2CDevicePtr dev, uint8_t reg_addr, uint8_t& data, int timeout){
	return i2c_master_transmit_receive(*dev, &reg_addr, 1, &data, 1, timeout) == ESP_OK;
}


bool I2CDriver::readFromMem(I2CDevicePtr dev, uint8_t reg_addr, uint16_t& data, int timeout){
	bool ret = i2c_master_transmit_receive(*dev, &reg_addr, 1, (uint8_t*)&data, 2, timeout) == ESP_OK;
	data = __builtin_bswap16(data);
	return ret;
}


bool I2CDriver::readFromMem(I2CDevicePtr dev, uint8_t reg_addr, uint32_t& data, int timeout){
	bool ret = i2c_master_transmit_receive(*dev, &reg_addr, 1, (uint8_t*)&data, 4, timeout) == ESP_OK;
	data = __builtin_bswap32(data);
	return ret;
}


bool I2CDriver::readFromMem(I2CDevicePtr dev, uint8_t reg_addr, uint8_t* data, size_t len, int timeout){
	return i2c_master_transmit_receive(*dev, &reg_addr, 1, data, len, timeout) == ESP_OK;
}

/********************************************************/
bool I2CDriver::writeToMem(I2CDevicePtr dev, uint8_t reg_addr, int8_t data, int timeout){
	 uint8_t buff[2] = {reg_addr, (uint8_t)data};
	return i2c_master_transmit(*dev, buff, 2, timeout) == ESP_OK;
}


bool I2CDriver::writeToMem(I2CDevicePtr dev, uint8_t reg_addr, int16_t data, int timeout){
	uint8_t* pdata = (uint8_t*)&data;
	uint8_t buff[3] = {reg_addr, pdata[1], pdata[0]};
	return i2c_master_transmit(*dev, buff, 3, timeout) == ESP_OK;
}


bool I2CDriver::writeToMem(I2CDevicePtr dev, uint8_t reg_addr, int32_t data, int timeout){
	uint8_t* pdata = (uint8_t*)&data;
	uint8_t buff[5] = {reg_addr, pdata[3], pdata[2], pdata[1], pdata[0]};
	return i2c_master_transmit(*dev, buff, 5, timeout) == ESP_OK;
}


bool I2CDriver::writeToMem(I2CDevicePtr dev, uint8_t reg_addr, uint8_t data, int timeout){
	 uint8_t buff[2] = {reg_addr, data};
	return i2c_master_transmit(*dev, buff, 2, timeout) == ESP_OK;
}


bool I2CDriver::writeToMem(I2CDevicePtr dev, uint8_t reg_addr, uint16_t data, int timeout){
	uint8_t* pdata = (uint8_t*)&data;
	uint8_t buff[3] = {reg_addr, pdata[1], pdata[0]};
	return i2c_master_transmit(*dev, buff, 3, timeout) == ESP_OK;
}


bool I2CDriver::writeToMem(I2CDevicePtr dev, uint8_t reg_addr, uint32_t data, int timeout){
	uint8_t* pdata = (uint8_t*)&data;
	uint8_t buff[5] = {reg_addr, pdata[3], pdata[2], pdata[1], pdata[0]};
	return i2c_master_transmit(*dev, buff, 5, timeout) == ESP_OK;
}

bool I2CDriver::write(I2CDevicePtr dev, const uint8_t* data, size_t len, int timeout){
	return i2c_master_transmit(*dev, data, len, timeout) == ESP_OK;
}

void I2CDriver::detect(){
	uint8_t address;
	printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");
	for (int i = 0; i < 128; i+= 16) {
		printf("%02x: ", i);
		for (int j = 0; j < 16; ++j) {
			fflush(stdout);
			address = i + j;
			esp_err_t ret = i2c_master_probe(this->bus, address, 100);
			if (ret == ESP_OK)               printf("%02x ", address);
			else if (ret == ESP_ERR_TIMEOUT) printf("UU ");
			else                             printf("-- ");
		}
		printf("\r\n");
	}
	printf("\r\n");
}
