#pragma once

#include "hal.h"
#include "base.h"
#include "sensors.h"

namespace minibot{

extern const float PI;
extern const float TWO_PI;
extern const float PI_2;
extern const float PI_4;

const char* get_light_quadrant_name(float ls_angle);
void get_light_source(float& ls_angle, float& ls_strength);

}