#pragma once
#include <memory>
#include <cstdint>

#include <esp_log.h>

#include "hal.h"
#include "i2c.h"
#include "encoders.h"

namespace minibot{

class   Base;
typedef std::shared_ptr<Base> BasePtr;

class Base{
	/* ** *************************************************************
	* Constructors & Singleton
	** ** ************************************************************/
public:
	static
	BasePtr getInstance(I2CDriverPtr i2c = NULL);

private:
	Base(I2CDriverPtr i2c);
	Base(const Base&)            = delete;
	Base& operator=(const Base&) = delete;

	/* ** *************************************************************
	* Getter methods
	** ** ************************************************************/
public:
	void getDistPIDConstants(float& kp, float& ki, float& kd) const;
	void getMoveTimeLimits(int32_t& minTime, int32_t& maxTime) const;
	void getPwm(float& l, float& r, float& f, float& b) const;
	void getSpeedPIDConstants(float& kp, float& ki, float& kd) const;
	void getSpeed(float& l, float& r, float& f, float& b) const;
	void getPWMCaps(uint16_t& upper, uint16_t& lower) const;

	/* ** *************************************************************
	* Setter methods
	** ** ************************************************************/
	void setDistPIDConstants(float kp, float ki, float kd);
	void setMoveTimeLimits(int32_t minMoveTime=1000, int32_t maxMoveTime=3000);
	void setSpeedPIDConstants(float kp, float ki, float kd);
	void setPWMCaps(uint16_t upper=3000, uint16_t lower=1800);

	/* ** *************************************************************
	* Methods
	** ** ************************************************************/
	void stop();
	void mv(float distance, float angle);
	void mv(float distance, float angle, float& od, float& oa);
	float moveX(float distance);
	float moveY(float distance);
	float rotate(float angle);

	bool setPwm(float left, float right, float front = 0.0f, float back = 0.0f);
	bool setSpeed(float left, float right, float front = 0.0f, float back = 0.0f);

	void readBatt(sbatteryr_t&);
	float readBattVolt();
	float readBattPerc();

private:
	Encoders readEncodersAbs();
	Encoders readEncodersDt();
	void waitStop();
	int32_t estimateMoveTimeMs(int32_t estSteps);

public:

	/* ** *************************************************************
	* Attributes
	** ** ************************************************************/
private:
	const uint8_t addr = 0x26;
	I2CDevicePtr dev;
	I2CDriverPtr i2c;

	// Last used values
	float pwml, pwmr, pwmf, pwmb;
	float spdl, spdr, spdf, spdb;

	// PID Constants - Distance
	float dKP, dKI, dKD;

	// PID Constants - Speed
	float sKP, sKI, sKD;

	volatile bool abortMove;
	int32_t minMoveTime, maxMoveTime;
	uint16_t pwmMin, pwmMax;

private:
	static BasePtr instance;
};

}; // End namespace

