#include "settings.h"

// namespace ns = minibot::settings;
namespace ns = minibot::settings;

// Wireless
// std::string WIFI_MODE = "ap";
// std::string WIFI_SSID = "Minibot";
// std::string WIFI_PASS = "12345678";
// std::string WIFI_AUTH = "wpa2";
// std::string WIFI_HOST = "minibot.local";
// std::string WIFI_ADDR = "192.168.1.1/24";
// std::string WIFI_GATE = "192.168.1.1";
// std::string WIFI_DNS  = "192.168.1.1";
std::string ns::WIFI_MODE = "client";
std::string ns::WIFI_SSID = "INFINITUM065E";
std::string ns::WIFI_PASS = "8173766338";
std::string ns::WIFI_AUTH = "wpa2";
std::string ns::WIFI_HOST = "minibot.local";
std::string ns::WIFI_ADDR = "";
std::string ns::WIFI_GATE = "";
std::string ns::WIFI_DNS  = "";

// Sensors
float    ns::SEN_DIST[]   = {60,0,300,330,-1,30};
float    ns::SEN_LIGHT[]  = {0,315,270,225,180,135,90,45};
float    ns::SEN_FLOOR[]  = {3,2,1,0,-1,-1,-1,-1};

// Base control
uint16_t ns::PWM_MIN      = 1800;
uint16_t ns::PWM_MAX      = 3000;
float    ns::PCTRL_KP     = 0.002;
float    ns::PCTRL_KI     = 0.000001;
float    ns::PCTRL_KD     = 0.0002;
float    ns::SCTRL_KP     = 0.0;
float    ns::SCTRL_KI     = 0.0;
float    ns::SCTRL_KD     = 0.0;

int16_t  ns::MOVE_MS_MIN  = 1000;
int16_t  ns::MOVE_MS_MAX  = 3000;

void ns::load(const char* file){
}
