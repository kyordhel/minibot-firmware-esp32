#include "minibot.h"
#include <cmath>

namespace minibot{

const float PI     = 3.141592653589793;
const float TWO_PI = 2 * PI;
const float PI_2   = PI / 2.0;
const float PI_4   = PI / 4.0;

static const char QUADRANTS[8][16] = {
	"right",
	"front-right",
	"front",
	"front-left",
	"left",
	"back-left",
	"back",
	"back-right",
};

const char* get_light_quadrant_name(float ls_angle){
	ls_angle+= 0.3927; //22.5° | Half sensor cone
	while (ls_angle <  0)        ls_angle+= TWO_PI;
	while (ls_angle >= TWO_PI) ls_angle-= TWO_PI;
	int qi = 8.0 * ls_angle / TWO_PI;
	return QUADRANTS[qi];
}

void get_light_source(float& ls_angle, float& ls_strength){
	ls_angle = 0;
	ls_strength = 0;

	slightr_t reading;
	if( !Sensors::getInstance()->readLight(reading) )
		return;

	float x = 0;
	float y = 0;
	for(uint8_t i = 0; i < LIGHT_SENSOR_COUNT; ++i){
		x+= reading.data[i].value * cos(reading.data[i].angle);
		y+= reading.data[i].value * sin(reading.data[i].angle);
	}
	ls_angle    = atan2(y, x);
	ls_strength = sqrt(x*x + y*y);
}

};