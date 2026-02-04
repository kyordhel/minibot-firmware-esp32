#pragma once
#include <memory>
#include <cstdint>

#include <esp_log.h>

#include "hal.h"
#include "i2c.h"

namespace minibot{

class   Sensors;
typedef std::shared_ptr<Sensors> SensorsPtr;

class Sensors{
	/* ** *************************************************************
	* Constructors & Singleton
	** ** ************************************************************/
public:
	static
	SensorsPtr getInstance(I2CDriverPtr i2c = NULL);

private:
	Sensors(I2CDriverPtr i2c);
	Sensors(const Sensors&)            = delete;
	Sensors& operator=(const Sensors&) = delete;

public:
	/* ** *************************************************************
	* Getter methods
	** ** ************************************************************/
	void getDistSenAngles(float values[DIST_SENSOR_COUNT]);
	void getLightSenAngles(float values[LIGHT_SENSOR_COUNT]);
	void getFloorSenOrder(uint8_t values[FLOOR_SENSOR_COUNT]);

	/* ** *************************************************************
	* Setter methods
	** ** ************************************************************/
	void setDistSenAngles(const float values[DIST_SENSOR_COUNT]);
	void setDistSenAnglesDeg(const float values[DIST_SENSOR_COUNT]);
	void setDistSenAnglesDeg(const char* str);
	void setLightSenAngles(const float values[LIGHT_SENSOR_COUNT]);
	void setLightSenAnglesDeg(const float values[LIGHT_SENSOR_COUNT]);
	void setLightSenAnglesDeg(const char* str);
	void setFloorSenOrder(const uint8_t values[FLOOR_SENSOR_COUNT]);
	void setFloorSenOrder(const char* str);

	/* ** *************************************************************
	* Methods
	** ** ************************************************************/
	bool readLight(slightr_t& reading);
	bool readLightRaw(uint8_t values[LIGHT_SENSOR_COUNT]);
	bool readFloor(sfloorr_t& reading);
	bool readFloorRaw(uint8_t values[FLOOR_SENSOR_COUNT]);
	bool readDistance(sdistr_t& reading);
	bool readDistanceRaw(uint8_t values[DIST_SENSOR_COUNT]);

private:
	bool readADCChannel(I2CDevicePtr dev, uint8_t ch, uint8_t& value);
	bool readADCRange(I2CDevicePtr dev, uint8_t first_ch, uint8_t last_ch, uint8_t* values);


	/* ** *************************************************************
	* Attributes
	** ** ************************************************************/
private:
	const uint8_t addr = 0x26;
	I2CDevicePtr devD[2], devF, devL;
	I2CDriverPtr i2c;
	float  dsAngles[DIST_SENSOR_COUNT];
	float  lsAngles[LIGHT_SENSOR_COUNT];
	int8_t fsOrder[FLOOR_SENSOR_COUNT];

private:
	static SensorsPtr instance;
};

}; // End namespace

