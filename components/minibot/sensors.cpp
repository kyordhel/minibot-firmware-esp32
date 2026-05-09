#include "sensors.h"
#include <cmath>
#include <sdkconfig.h>

using namespace minibot;

const uint8_t LIGHT_SEN_ADDR       = 0x48;
const uint8_t DIST_SEN_ADDR[2]     = {0x49, 0x4a};
const uint8_t FLOOR_SEN_ADDR       = 0x4b;

const uint8_t ADS7830_SINGLE       = 0x80;
const uint8_t ADS7830_DIFFERENTIAL = 0x00;
const uint8_t ADS7830_CHANNEL0     = 0x00;
const uint8_t ADS7830_CHANNEL1     = 0x10;
const uint8_t ADS7830_CHANNEL2     = 0x20;
const uint8_t ADS7830_CHANNEL3     = 0x30;
const uint8_t ADS7830_CHANNEL4     = 0x40;
const uint8_t ADS7830_CHANNEL5     = 0x50;
const uint8_t ADS7830_CHANNEL6     = 0x60;
const uint8_t ADS7830_CHANNEL7     = 0x70;
const uint8_t ADS7830_POWER_DOWN   = 0x00;
const uint8_t ADS7830_POWER_ADIR   = 0x0c;
const uint8_t ADS7830_POWER_AD_ON  = 0x04;
const uint8_t ADS7830_POWER_AD_OFF = 0x00;
const uint8_t ADS7830_POWER_IR_ON  = 0x08;
const uint8_t ADS7830_POWER_IR_OFF = 0x00;
static const float __VTD[256]      = {
    //      0x00,    0x01,    0x02,    0x03,    0x04,    0x05,    0x06,    0x07,    0x08,    0x09,    0x0a,    0x0b,    0x0c,    0x0d,    0x0e,    0x0f
/*0x00*/ -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000,
/*0x10*/ -1.0000,  0.1803,  0.1703,  0.1613,  0.1531,  0.1457,  0.1390,  0.1328,  0.1271,  0.1218,  0.1169,  0.1123,  0.1081,  0.1041,  0.1004,  0.0969,
/*0x20*/  0.0937,  0.0906,  0.0877,  0.0849,  0.0823,  0.0798,  0.0775,  0.0752,  0.0731,  0.0711,  0.0691,  0.0673,  0.0655,  0.0638,  0.0622,  0.0606,
/*0x30*/  0.0591,  0.0577,  0.0563,  0.0549,  0.0536,  0.0524,  0.0512,  0.0500,  0.0489,  0.0478,  0.0468,  0.0458,  0.0448,  0.0438,  0.0429,  0.0420,
/*0x40*/  0.0412,  0.0403,  0.0395,  0.0387,  0.0379,  0.0372,  0.0365,  0.0358,  0.0351,  0.0344,  0.0337,  0.0331,  0.0325,  0.0319,  0.0313,  0.0307,
/*0x50*/  0.0301,  0.0296,  0.0290,  0.0285,  0.0280,  0.0275,  0.0270,  0.0265,  0.0260,  0.0256,  0.0251,  0.0247,  0.0242,  0.0238,  0.0234,  0.0230,
/*0x60*/  0.0226,  0.0222,  0.0218,  0.0214,  0.0211,  0.0207,  0.0204,  0.0200,  0.0197,  0.0193,  0.0190,  0.0187,  0.0184,  0.0180,  0.0177,  0.0174,
/*0x70*/  0.0171,  0.0168,  0.0166,  0.0163,  0.0160,  0.0157,  0.0155,  0.0152,  0.0149,  0.0147,  0.0144,  0.0142,  0.0139,  0.0137,  0.0134,  0.0132,
/*0x80*/  0.0130,  0.0127,  0.0125,  0.0123,  0.0121, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000,
/*0x90*/ -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000,
/*0xa0*/ -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000,
/*0xb0*/ -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000,
/*0xc0*/ -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000,
/*0xd0*/ -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000,
/*0xe0*/ -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000,
/*0xf0*/ -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000, -1.0000,
};

/* ** *************************************************************
* Helper function prototypes
** ** ************************************************************/
static inline uint8_t adc_ch2C(uint8_t);
static inline float deg2rad(float);

/* ** *************************************************************
* Singleton
** ** ************************************************************/
SensorsPtr Sensors::instance = nullptr;

SensorsPtr Sensors::getInstance(I2CDriverPtr i2c){
	if (Sensors::instance == nullptr)
		Sensors::instance = std::shared_ptr<Sensors>(new Sensors(i2c));
	return Sensors::instance;
}


/* ** *************************************************************
* Constructors
** ** ************************************************************/
Sensors::Sensors(I2CDriverPtr i2c) : i2c(i2c),
	dsAngles{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	lsAngles{-1,-1,-1,-1,-1,-1,-1,-1},
	fsOrder{3,2,1,0}{
		this->devD[0] = i2c->addDevice(DIST_SEN_ADDR[0]);
		this->devD[1] = i2c->addDevice(DIST_SEN_ADDR[1]);
		this->devL = i2c->addDevice(LIGHT_SEN_ADDR);
		this->devF = i2c->addDevice(FLOOR_SEN_ADDR);
		this->setFloorSenOrder(CONFIG_SEN_FLOOR);
		this->setDistSenAnglesDeg(CONFIG_SEN_DIST);
		this->setLightSenAnglesDeg(CONFIG_SEN_LIGHT);
	}


/* ** *************************************************************
* Getter & Setter methods
** ** ************************************************************/
void Sensors::getDistSenAngles(float values[DIST_SENSOR_COUNT]){
	for(uint8_t i = 0; i < DIST_SENSOR_COUNT; ++i)
		values[i] = dsAngles[i];
}


void Sensors::setDistSenAngles(const float values[DIST_SENSOR_COUNT]){
	for(uint8_t i = 0; i < DIST_SENSOR_COUNT; ++i)
		dsAngles[i] = values[i];
}

void Sensors::setDistSenAnglesDeg(const float values[DIST_SENSOR_COUNT]){
	for(uint8_t i = 0; i < DIST_SENSOR_COUNT; ++i)
		dsAngles[i] = deg2rad(values[i]);
}


void Sensors::setDistSenAnglesDeg(const char* str){
	float deg;
	uint8_t i = 0;
	char *bcc, *cc = (char*)str, *end = cc + strlen(str);

	while( (cc < end) && (i < DIST_SENSOR_COUNT) ){
		deg = strtof(cc, &bcc);
		if(bcc != cc){
			dsAngles[i++] = (deg >= 0) ? deg2rad(deg) : -1;
			cc = bcc;
		}
		else ++cc;
	}
	while( i < LIGHT_SENSOR_COUNT ) dsAngles[i++] = -1;
}


void Sensors::getLightSenAngles(float values[LIGHT_SENSOR_COUNT]){
	for(uint8_t i = 0; i < LIGHT_SENSOR_COUNT; ++i)
		values[i] = lsAngles[i];
}


void Sensors::setLightSenAngles(const float values[LIGHT_SENSOR_COUNT]){
	for(uint8_t i = 0; i < LIGHT_SENSOR_COUNT; ++i)
		lsAngles[i] = values[i];
}

void Sensors::setLightSenAnglesDeg(const float values[LIGHT_SENSOR_COUNT]){
	for(uint8_t i = 0; i < LIGHT_SENSOR_COUNT; ++i)
		lsAngles[i] = deg2rad(values[i]);
}


void Sensors::setLightSenAnglesDeg(const char* str){
	float deg;
	uint8_t i = 0;
	char *bcc, *cc = (char*)str, *end = cc + strlen(str);

	while( (cc < end) && (i < LIGHT_SENSOR_COUNT) ){
		deg = strtof(cc, &bcc);
		if(bcc != cc){
			lsAngles[i++] = (deg >= 0) ? deg2rad(deg) : -1;
			cc = bcc;
		}
		else ++cc;
	}
	while( i < LIGHT_SENSOR_COUNT ) lsAngles[i++] = -1;
}


void Sensors::getFloorSenOrder(uint8_t values[4]){
	values[0] = fsOrder[0]; values[1] = fsOrder[1];
	values[2] = fsOrder[2]; values[3] = fsOrder[3];
}


void Sensors::setFloorSenOrder(const uint8_t values[4]){
	fsOrder[0] = values[0]; fsOrder[1] = values[1];
	fsOrder[2] = values[2]; fsOrder[3] = values[3];
}


void Sensors::setFloorSenOrder(const char* str){
	long n;
	uint8_t i = 0;
	char *bcc, *cc = (char*)str, *end = cc + strlen(str);

	while( (cc < end) && (i < FLOOR_SENSOR_COUNT) ){
		n = strtol(cc, &bcc, 10);
		if(bcc != cc){
			fsOrder[i++] = ((n >= 0) && (n < FLOOR_SENSOR_COUNT)) ? n : -1;
			cc = bcc;
		}
		else ++cc;
	}
	while( i < FLOOR_SENSOR_COUNT ) fsOrder[i++] = -1;
}


/* ** *************************************************************
* Methods
** ** ************************************************************/
bool Sensors::readLight(slightr_t& reading){
	uint8_t memaddr, value, ok = 0;
	for( uint8_t i = 0; i < LIGHT_SENSOR_COUNT; ++i ){
		reading.data[i].value = -1;
		reading.data[i].angle = -1;
	}

	for( uint8_t i = 0, j = 0; i < LIGHT_SENSOR_COUNT; ++i ){
		if( lsAngles[i] < 0) continue;
		memaddr = ADS7830_SINGLE | ADS7830_POWER_AD_ON | adc_ch2C(i);
		reading.data[j].angle = lsAngles[i];
		if( i2c->readFromMem(devL, memaddr, value) ){
			reading.data[j].value = value / 255.0f;
			++ok;
		}
		else reading.data[j].value = -1;
		++j;
	}
	return ok == LIGHT_SENSOR_COUNT;
}


bool Sensors::readLightRaw(uint8_t values[LIGHT_SENSOR_COUNT]){
	uint8_t memaddr, ok = 0;
	for( uint8_t i = 0; i < LIGHT_SENSOR_COUNT; ++i ) values[i] = -1;
	for( uint8_t i = 0, j = 0; i < LIGHT_SENSOR_COUNT; ++i ){
		memaddr = ADS7830_SINGLE | ADS7830_POWER_AD_ON | adc_ch2C(i);
		if( lsAngles[i] < 0) continue;
		if( i2c->readFromMem(devL, memaddr, values[j]) ) ++ok;
		else values[j] = -1;
		++j;
	}
	return ok == LIGHT_SENSOR_COUNT;
}


bool Sensors::readFloor(sfloorr_t& reading){
	uint8_t memaddr, value, ok = 0;
	for( uint8_t i = 0; i < FLOOR_SENSOR_COUNT; ++i )
		reading.data[i] = -1;

	for( uint8_t i = 0; i < FLOOR_SENSOR_COUNT; ++i ){
		if( fsOrder[i] < 0) continue;
		memaddr = ADS7830_SINGLE | ADS7830_POWER_AD_ON | adc_ch2C(i);
		if( i2c->readFromMem(devF, memaddr, value) ){
			reading.data[fsOrder[i]] = value / 255.0f;
			++ok;
		}
		else reading.data[fsOrder[i]] = -1;
	}
	return ok == FLOOR_SENSOR_COUNT;
}


bool Sensors::readFloorRaw(uint8_t values[FLOOR_SENSOR_COUNT]){
	uint8_t memaddr, ok = 0;
	for( uint8_t i = 0; i < FLOOR_SENSOR_COUNT; ++i )
		values[i] = -1;

	for( uint8_t i = 0; i < FLOOR_SENSOR_COUNT; ++i ){
		if( fsOrder[i] < 0) continue;
		memaddr = ADS7830_SINGLE | ADS7830_POWER_AD_ON | adc_ch2C(i);
		if( i2c->readFromMem(devF, memaddr, values[fsOrder[i]]) ) ++ok;
		else values[fsOrder[i]] = -1;
	}
	return ok == FLOOR_SENSOR_COUNT;
}


bool Sensors::readDistance(sdistr_t& reading){
	float voltage;
	uint8_t memaddr, value, expected = 0, ok = 0;

	for( uint8_t i = 0; i < DIST_SENSOR_COUNT; ++i ){
		reading.data[i].angle = -1;
		reading.data[i].value = -1;
	}

	reading.count = 0;
	for( uint8_t i = 0; i < DIST_SENSOR_COUNT; ++i ){
		if( dsAngles[i] < 0) continue;

		++expected;
		reading.data[reading.count].angle = dsAngles[i];
		memaddr = ADS7830_SINGLE | ADS7830_POWER_AD_ON | adc_ch2C(i % 8);
		if( !i2c->readFromMem(devD[i/8], memaddr, value) ){
			reading.data[reading.count++].value = -1;
			continue;
		}

		++ok;
		/** Deprecated code:
		 * Non-linear interpolation is computationally expensive;
		 * moreso without an fPU. Thus it has been replaced with a
		 * O(1) lookup in a long array __VTD
		 **************************************************************
		 * voltage = 5.0f * value / 255.0f;
		 * if( (voltage < 0.33) || (voltage > 2.60) ){
		 * 	reading.data[reading.count++].value = -1;
		 * 	continue;
		 * }
		 * reading.data[reading.count++].value = 0.074634 * std::pow(voltage, -0.8972) - 0.019711;
		*/
		reading.data[reading.count++].value = __VTD[value];
	}
	return expected == ok;
}


bool Sensors::readDistanceRaw(uint8_t values[DIST_SENSOR_COUNT]){
	uint8_t memaddr, expected = 0, ok = 0;
	for( uint8_t i = 0; i < DIST_SENSOR_COUNT; ++i )
		values[i] = -1;

	for( uint8_t i = 0, j = 0; i < DIST_SENSOR_COUNT; ++i ){
		if( dsAngles[i] < 0) continue;
		++expected;
		memaddr = ADS7830_SINGLE | ADS7830_POWER_AD_ON | adc_ch2C(i % 8);
		if( i2c->readFromMem(devD[i/8], memaddr, values[j]) ) ++ok;
		else values[j] = -1;
		++j;
	}
	return ok == expected;
}


bool Sensors::readADCChannel(I2CDevicePtr dev, uint8_t ch, uint8_t& value){
	uint8_t memaddr = ADS7830_SINGLE | ADS7830_POWER_AD_ON | adc_ch2C(ch);
	return i2c->readFromMem(dev, memaddr, value);
}


bool Sensors::readADCRange(I2CDevicePtr dev, uint8_t first_ch, uint8_t last_ch, uint8_t* values){
	uint8_t memaddr, ok = 0;
	for( uint8_t i = 0, ch = first_ch; ch <= last_ch; ++i, ++ch){
		values[i] = -1;
		memaddr = ADS7830_SINGLE | ADS7830_POWER_AD_ON | adc_ch2C(ch);
		if( i2c->readFromMem(dev, memaddr, values[i]) ) ++ok;
		else values[i] = -1;
	}
	return ok == (1+last_ch - first_ch);
}


/* ** *************************************************************
* Helper function definitions
** ** ************************************************************/
static inline
uint8_t adc_ch2C(uint8_t ch){
	/*
	 * From Datasheet page 14 with SD=1:
	 * Ch C[2:0]    Ch C[2:0]
	 *  0   000      1   100
	 *  2   001      3   101
	 *  4   010      5   110
	 *  6   011      7   111
	 *  => C = (ch%2 == 0) ? ch / 2 : ch / 2 | 0x04
	*/
	return ( (ch % 2) == 0 ? ch / 2 : ch / 2 | 0x04 ) << 4;
}

static inline
float deg2rad(float deg){
	return deg * 0.017453292519943295;
}

