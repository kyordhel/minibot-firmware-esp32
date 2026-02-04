#pragma once
#include <cstdint>

namespace minibot{

const uint8_t FLOOR_SENSOR_COUNT = 4;
const uint8_t LIGHT_SENSOR_COUNT = 8;
const uint16_t DIST_SENSOR_COUNT = 16;

/**
 * Base struct for sensor measurement.
 * Packs the sensor angle and the read value
 * -1 is an invalid/null measurement
 */
struct __attribute__((__packed__)){
	float angle;
	float value;
} typedef avtuple_t;

/**
 * Floor measurement.
 * Packs the values of the four sensors from left to right
 * -1 is an invalid/null measurement
 */
struct __attribute__((__packed__)){
	float data[FLOOR_SENSOR_COUNT];
}typedef sfloorr_t;

/**
 * Light measurement.
 * Packs the values of the eight light sensors with their angles
 * -1 in either angle or value is an invalid/null measurement
 */
struct __attribute__((__packed__)){
	avtuple_t data[LIGHT_SENSOR_COUNT];
}typedef slightr_t;


/**
 * Distance measurement.
 * Packs the values of all the distance sensors with their angles
 * -1 in either angle or value is an invalid/null measurement
 */
struct __attribute__((__packed__)){
	uint16_t count;
	avtuple_t data[DIST_SENSOR_COUNT];
}typedef sdistr_t;

/**
 * Battery measurement. Percentage ranges from 0 to 100.
 * -1 is an invalid/null measurement
 */
struct __attribute__((__packed__)){
	float  voltage;
	int8_t percentage;
}typedef sbatteryr_t;


struct __attribute__((__packed__)){
	sbatteryr_t batt;
	sfloorr_t   floor;
	slightr_t   light;
	sdistr_t    dist;
}typedef sall_t;

}; // End namespace