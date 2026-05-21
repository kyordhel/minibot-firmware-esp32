#pragma once
#ifndef __MINIBOT_SETTINGS_H__
#define __MINIBOT_SETTINGS_H__

#include <string>
#include <cstdint>
#include "hal.h"

namespace minibot::settings{
// Wireless
extern std::string WIFI_MODE;
extern std::string WIFI_SSID;
extern std::string WIFI_PASS;
extern std::string WIFI_AUTH;
extern std::string WIFI_HOST;
extern std::string WIFI_ADDR;
extern std::string WIFI_GATE;
extern std::string WIFI_DNS;

// Sensors
extern float       SEN_DIST[DIST_SENSOR_COUNT];
extern float       SEN_LIGHT[LIGHT_SENSOR_COUNT];
extern uint8_t     SEN_FLOOR[FLOOR_SENSOR_COUNT];

// Base control
extern uint16_t PWM_MIN;
extern uint16_t PWM_MAX;
extern float    PCTRL_KP;
extern float    PCTRL_KI;
extern float    PCTRL_KD;
extern float    SCTRL_KP;
extern float    SCTRL_KI;
extern float    SCTRL_KD;

extern int16_t  MOVE_MS_MIN;
extern int16_t  MOVE_MS_MAX;


void load(const char* file);

}// End namespace

#endif // __MINIBOT_SETTINGS_H__