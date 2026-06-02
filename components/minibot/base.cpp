#include "base.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

using namespace minibot;

static const uint8_t DRV_MOTOR_TYPE   = 0x01;
static const uint8_t DRV_DEADBAND     = 0x02;
static const uint8_t DRV_RING_LINES   = 0x03;
static const uint8_t DRV_MREDUCTION   = 0x04;
static const uint8_t DRV_WDIAMETER    = 0x05;
static const uint8_t DRV_SPEED        = 0x06;
static const uint8_t DRV_PWM          = 0x07;
static const uint8_t DRV_BATTERY      = 0x08;
static const uint8_t DRV_M1_ENC_RT    = 0x10;
static const uint8_t DRV_M2_ENC_RT    = 0x11;
static const uint8_t DRV_M3_ENC_RT    = 0x12;
static const uint8_t DRV_M4_ENC_RT    = 0x13;
static const uint8_t DRV_M1_ENC_CNT_H = 0x20;
static const uint8_t DRV_M1_ENC_CNT_L = 0x21;
static const uint8_t DRV_M2_ENC_CNT_H = 0x22;
static const uint8_t DRV_M2_ENC_CNT_L = 0x23;
static const uint8_t DRV_M3_ENC_CNT_H = 0x24;
static const uint8_t DRV_M3_ENC_CNT_L = 0x25;
static const uint8_t DRV_M4_ENC_CNT_H = 0x26;
static const uint8_t DRV_M4_ENC_CNT_L = 0x27;

/* ** *************************************************************
* Helper function prototypes
** ** ************************************************************/
// template<typename T>
// static inline void clamp2one(T& value);
// template<typename T>
// static inline void clamp(T& value, T min, T max);
template<typename T>
static inline T clamp2one(T value);
template<typename T>
static inline T clamp(T value, T min, T max);
static inline float sign(float value);

/* ** *************************************************************
* Singleton
** ** ************************************************************/
BasePtr Base::instance = nullptr;

BasePtr Base::getInstance(I2CDriverPtr i2c) {
	if (Base::instance == nullptr)
		Base::instance = std::shared_ptr<Base>(new Base(i2c));
	return Base::instance;
}

/* ** *************************************************************
* Constructors
** ** ************************************************************/
Base::Base(I2CDriverPtr i2cDriver):
	addr(0x26), i2c(i2cDriver),
	pwml(0), pwmr(0), pwmf(0), pwmb(0),
	spdl(0), spdr(0), spdf(0), spdb(0),
	dKP(0.002f), dKI(0.000001f), dKD(0.0002f),
	sKP(0), sKI(0), sKD(0),
	abortMove(true){
	this->dev = i2c->addDevice(addr);
	this->setPWMCaps(3000, 1800);
	this->stop();
}

/* ** *************************************************************
* Getter methods
** ** ************************************************************/
void Base::getDistPIDConstants(float& kp, float& ki, float& kd) const {
	kp = this->dKP; ki = this->dKI; kd = this->dKD;
}

void Base::getMoveTimeLimits(int32_t& minTime, int32_t& maxTime) const {
	minTime = this->minMoveTime; maxTime = this->maxMoveTime;
}

void Base::getPwm(float& l, float& r, float& f, float& b) const {
	l = this->pwml; r = this->pwmr; f = this->pwmf; b = this->pwmb;
}

void Base::getSpeedPIDConstants(float& kp, float& ki, float& kd) const {
	kp = this->sKP; ki = this->sKI; kd = this->sKD;
}

void Base::getSpeed(float& l, float& r, float& f, float& b) const {
	l = this->spdl; r = this->spdr; f = this->spdf; b = this->spdb;
}


/* ** *************************************************************
* Setter methods
** ** ************************************************************/
void Base::setDistPIDConstants(float kp, float ki, float kd) {
	this->dKP = kp; this->dKI = ki; this->dKD = kd;
}


void Base::setMoveTimeLimits(int32_t minMoveTime, int32_t maxMoveTime) {
	if (minMoveTime < 0) minMoveTime = 2147483647;
	if (maxMoveTime < 0) maxMoveTime = 2147483647;
	if (maxMoveTime < minMoveTime) maxMoveTime = minMoveTime;

	this->minMoveTime = minMoveTime;
	this->maxMoveTime = maxMoveTime;
}


bool Base::setPwm(float left, float right, float front, float back) {
	this->pwml = clamp2one(left);
	this->pwmr = clamp2one(right);
	this->pwmf = clamp2one(front);
	this->pwmb = clamp2one(back);

	int16_t l = sign(pwml) * (int16_t)( pwmMin + (pwmMax-pwmMin) * abs(pwml) );
	int16_t r = sign(pwmr) * (int16_t)( pwmMin + (pwmMax-pwmMin) * abs(pwmr) );
	int16_t f = sign(pwmf) * (int16_t)( pwmMin + (pwmMax-pwmMin) * abs(pwmf) );
	int16_t b = sign(pwmb) * (int16_t)( pwmMin + (pwmMax-pwmMin) * abs(pwmb) );

	uint8_t buff[9] = { DRV_PWM,
		((uint8_t*)&l)[1], ((uint8_t*)&l)[0],
		((uint8_t*)&f)[1], ((uint8_t*)&f)[0],
		((uint8_t*)&b)[1], ((uint8_t*)&b)[0],
		((uint8_t*)&r)[1], ((uint8_t*)&r)[0]
	};
	return i2c->write(this->dev, buff, 9);
}


bool Base::setSpeed(float left, float right, float front, float back) {
	this->spdl = clamp2one(left);
	this->spdr = clamp2one(right);
	this->spdf = clamp2one(front);
	this->spdb = clamp2one(back);

	int16_t l = spdl * 1000;
	int16_t r = spdr * 1000;
	int16_t f = spdf * 1000;
	int16_t b = spdb * 1000;

	uint8_t buff[9] = { DRV_SPEED,
		((uint8_t*)&l)[0], ((uint8_t*)&l)[1],
		((uint8_t*)&f)[0], ((uint8_t*)&f)[1],
		((uint8_t*)&b)[0], ((uint8_t*)&b)[1],
		((uint8_t*)&r)[0], ((uint8_t*)&r)[1]
	};
	return i2c->write(this->dev, buff, 9);
}


void Base::setPWMCaps(uint16_t upper, uint16_t lower) {
	// Clamp to 0-3600 range
	pwmMax = clamp<uint16_t>(upper, 0, 3600);
	pwmMin = clamp<uint16_t>(lower, 0, 3600);
}

void Base::setSpeedPIDConstants(float kp, float ki, float kd) {
	this->sKP = kp; this->sKI = ki; this->sKD = kd;
}


/* ** *************************************************************
* Methods
** ** ************************************************************/
void Base::stop() {
	this->abortMove = true;
	this->setPwm(0, 0, 0, 0);
}


void Base::waitStop() {
	Encoders e;
	do{
		e = readEncodersDt();
		vTaskDelay( pdMS_TO_TICKS(100) );
	}
	while (e.average() != 0.0f);
}


void Base::mv(float distance, float angle){
	rotate(angle);
	moveX(distance);
}


void Base::mv(float distance, float angle, float& od, float& oa){
	oa = rotate(angle);
	od = moveX(distance);
}


float Base::moveY(float distance) {
	Encoders e0, ek, ef, err, err_, errI, errD;
	float pwmf, pwmb, errSum;
	// Motors f and b advance towards y
	float estSteps = distance * 6756.76f; // 0.185m = 1250 pulses

	stop();
	abortMove = false;

	e0 = readEncodersAbs();
	ef = e0 + Encoders(estSteps, estSteps, 0, 0);

	int maxTime = estimateMoveTimeMs(estSteps) + 1;

	for (int k = 0; (k < maxTime) && !abortMove; k += 10) {
		Encoders ek = readEncodersAbs();
		err_ = err;        // Backup (previous error)
		err = ef - ek;     // Current error
		errI += err;       // Integral
		errD = err - err_; // Differential

		pwmf = 0.85 * dKP * err.front + dKI * errI.front + dKD * errD.front;
		pwmb = 0.85 * dKP * err.back  + dKI * errI.back  + dKD * errD.back;

		errSum = std::abs(err.front) + std::abs(err.back);
		if ( (errSum / 2.0f) < 100.0f ) break;

		setPwm(0, 0, pwmf, pwmb);
		vTaskDelay( pdMS_TO_TICKS(10) );
	}

	stop();
	waitStop();

	Encoders diff = readEncodersAbs() - e0;
	return (diff.front + diff.back) / (2 * 6756.76f);
}


float Base::moveX(float distance) {
	Encoders e0, ek, ef, err, err_, errI, errD;
	float pwml, pwmr, errSum;
	float estSteps = distance * 6756.76f; // 0.185m = 1250 pulses

	stop();
	abortMove = false;

	e0 = readEncodersAbs();
	// Motors l and r advance towards -x
	ef = e0 + Encoders(0, 0, estSteps, estSteps);

	int maxTime = estimateMoveTimeMs(estSteps) + 1;

	for (int k = 0; (k < maxTime) && !abortMove; k += 10) {
		Encoders ek = readEncodersAbs();
		err_ = err;        // Backup (previous error)
		err  = ek - ef;    // Current error
		errI+= err;        // Integral
		errD = err - err_; // Differential

		pwml = 0.85 * dKP * err.left  + dKI * errI.left  + dKD * errD.left;
		pwmr = 0.85 * dKP * err.right + dKI * errI.right + dKD * errD.right;

		errSum = std::abs(err.left) + std::abs(err.right);
		if ( (errSum / 2.0f) < 100.0f ) break;

		setPwm(pwml, pwmr, 0, 0);
		vTaskDelay( pdMS_TO_TICKS(10) );
	}

	stop();
	waitStop();

	Encoders diff = readEncodersAbs() - e0;
	return (diff.left + diff.right) / (2 * 6756.76f);
}


float Base::rotate(float angle) {
	Encoders e0, ek, ef, err, err_, errI, errD;
	float pwmf, pwmb, pwml, pwmr, errSum;
	float estSteps = angle * 620.0f;

	stop();
	abortMove = false;

	e0 = readEncodersAbs();
	// l,r<- f,b↓  =>  +f +l -b -r
	ef = e0 + Encoders(-estSteps, estSteps, -estSteps, estSteps);

	int maxTime = estimateMoveTimeMs(estSteps) + 1;

	for (int k = 0; (k < maxTime) && !abortMove; k += 10) {
	// while (!abortMove) {
		Encoders ek = readEncodersAbs();
		err_ = err;        // Backup (previous error)
		err  = ek - ef;    // Current error
		errI+= err;        // Integral
		errD = err - err_; // Differential

		pwmf = 0.7 * dKP * err.front + dKI * errI.front + dKD * errD.front;
		pwmb = 0.7 * dKP * err.back  + dKI * errI.back  + dKD * errD.back;
		pwml = 0.7 * dKP * err.front + dKI * errI.front + dKD * errD.front;
		pwmr = 0.7 * dKP * err.back  + dKI * errI.back  + dKD * errD.back;

		errSum = std::abs(err.front) + std::abs(err.back) + std::abs(err.left) + std::abs(err.right);
		if ( (errSum / 4.0) < 100.0f ) break;

		setPwm(pwml, pwmr, pwmf, pwmb);
		vTaskDelay( pdMS_TO_TICKS(10) );
	}

	stop();
	waitStop();

	// Encoders diff = readEncodersAbs() - e0;
	Encoders ef2 = readEncodersAbs();
	Encoders diff = ef2 - e0;
	return (-diff.front + diff.back - diff.left + diff.right) / (620.0f * 4);
}


Encoders Base::readEncodersAbs() {
	union{
		int16_t i16;
		uint8_t i8[2];
	} typedef edata;
	// int16_t f, b, l, r;
	edata f, b, l, r;
	if (!i2c->readFromMem(dev, DRV_M1_ENC_CNT_H, &l.i8[0], 1)
		|| !i2c->readFromMem(dev, DRV_M1_ENC_CNT_L, &l.i8[1], 1)
		|| !i2c->readFromMem(dev, DRV_M2_ENC_CNT_H, &f.i8[0], 1)
		|| !i2c->readFromMem(dev, DRV_M2_ENC_CNT_L, &f.i8[1], 1)
		|| !i2c->readFromMem(dev, DRV_M3_ENC_CNT_H, &b.i8[0], 1)
		|| !i2c->readFromMem(dev, DRV_M3_ENC_CNT_L, &b.i8[1], 1)
		|| !i2c->readFromMem(dev, DRV_M4_ENC_CNT_H, &r.i8[0], 1)
		|| !i2c->readFromMem(dev, DRV_M4_ENC_CNT_L, &r.i8[1], 1)
	) return Encoders();
	return Encoders(f.i16, b.i16, l.i16, r.i16);
}

Encoders Base::readEncodersDt() {
	int16_t f, b, l, r;
	if (!i2c->readFromMem(dev, DRV_M1_ENC_RT, l)
		|| !i2c->readFromMem(dev, DRV_M2_ENC_RT, f)
		|| !i2c->readFromMem(dev, DRV_M3_ENC_RT, b)
		|| !i2c->readFromMem(dev, DRV_M4_ENC_RT, r)
	) return Encoders();
	return Encoders(f, b, l, r);
}


void Base::readBatt(sbatteryr_t& r) {
	r.voltage = readBattVolt();
	if      (r.voltage < 5.5f) r.percentage = 0;
	else if (r.voltage > 7.2f) r.percentage = 100;
	else                       r.percentage = (r.voltage - 5.5f) / 0.027f;
}

float Base::readBattVolt() {
	int16_t battV = 0;
	if( !i2c->readFromMem(this->dev, DRV_BATTERY, battV) ) return 0;
	return battV / 10.0f;
}


float Base::readBattPerc() {
	float battV = readBattVolt();
	if (battV < 5.5f) return 0.0f;
	else if (battV > 7.2f) return 100.0f;
	return (battV - 5.5f) / 0.027f;
}


int32_t Base::estimateMoveTimeMs(int32_t estSteps) {
	// Logic: 1.2 steps per ms + offset
	int32_t est = 100 + (int32_t)(std::abs(estSteps) / 1.2f);

	// Clamp between min and max
	clamp(est, this->minMoveTime, this->maxMoveTime);
	return est;
}

/* ** *************************************************************
* Helper function definitions
** ** ************************************************************/
template<typename T>
static inline
T clamp2one(T value){
	if( value < -1 ) return -1;
	if( value >  1 ) return 1;
	return value;
}

template<typename T>
static inline
T clamp(T value, T min, T max){
	if( value < min ) return min;
	if( value > max ) return max;
	return value;
}

static inline
float sign(float value){
	if (value == 0) return 0;
	return value >= 0 ? 1.0f : -1.0f;
}
