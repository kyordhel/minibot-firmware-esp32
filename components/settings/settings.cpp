#include "settings.h"

#include "base.h"
#include "sensors.h"

// namespace ns = minibot::settings;
namespace ns = minibot::settings;

// Wireless
#if defined(CONFIG_WIFI_MODE_CLIENT) && CONFIG_WIFI_MODE_CLIENT == true
	std::string ns::WIFI_MODE = "client";
#else
	#if !defined(CONFIG_WIFI_MODE_AP) || CONFIG_WIFI_MODE_AP != true
		#define CONFIG_WIFI_MODE_AP true
	#endif
	std::string ns::WIFI_MODE = "ap";
#endif

#ifdef CONFIG_WIFI_SSID
	std::string ns::WIFI_SSID = CONFIG_WIFI_SSID;
#else
	std::string ns::WIFI_SSID = "Minibot";
#endif

#ifdef CONFIG_WIFI_PASS
	std::string ns::WIFI_PASS = CONFIG_WIFI_PASS;
#else
	std::string ns::WIFI_PASS = "12345678";
#endif

#ifdef CONFIG_WIFI_AUTH
	std::string ns::WIFI_AUTH = CONFIG_WIFI_AUTH;
#else
	std::string ns::WIFI_AUTH = "wpa2";
#endif

#ifdef CONFIG_WIFI_HOST
	std::string ns::WIFI_HOST = CONFIG_WIFI_HOST;
#else
	std::string ns::WIFI_HOST = "minibot.local";
#endif

#ifdef CONFIG_WIFI_ADDR
	std::string ns::WIFI_ADDR = CONFIG_WIFI_ADDR;
#else
	#ifdef CONFIG_WIFI_MODE_AP
		std::string ns::WIFI_ADDR = "192.168.1.1/24";
	#else
		std::string ns::WIFI_ADDR = "";
	#endif
#endif

#ifdef CONFIG_WIFI_GATE
	std::string ns::WIFI_GATE = CONFIG_WIFI_GATE;
#else
	#ifdef CONFIG_WIFI_MODE_AP
		std::string ns::WIFI_GATE = "192.168.1.1";
	#else
		std::string ns::WIFI_GATE = "";
	#endif
#endif

#ifdef CONFIG_WIFI_DNS
	std::string ns::WIFI_DNS = CONFIG_WIFI_DNS;
#else
	#ifdef CONFIG_WIFI_MODE_AP
		std::string ns::WIFI_DNS = "192.168.1.1";
	#else
		std::string ns::WIFI_DNS = "";
	#endif
#endif

// std::string ns::WIFI_SSID = "INFINITUM065E";
// std::string ns::WIFI_PASS = "8173766338";


// Sensors
// float    ns::SEN_DIST[DIST_SENSOR_COUNT]  = {120, 90, 60, 30, 0, 330, -1, -1, 300, 270, 240, 210, 180, 150, -1, -1};
// float    ns::SEN_FLOOR[LIGHT_SENSOR_COUNT] = {3,2,1,0,-1,-1,-1,-1};
// float    ns::SEN_LIGHT[FLOOR_SENSOR_COUNT] = {90,45,0,315,270,225,180,135};

float    ns::SEN_DIST[]  = {120, 90, 60, 30, 0, 330, -1, -1, 300, 270, 240, 210, 180, 150, -1, -1};
uint8_t  ns::SEN_FLOOR[] = {3,2,1,0};
float    ns::SEN_LIGHT[] = {90,45,0,315,270,225,180,135};


#ifdef CONFIG_PWM_MIN
	uint16_t ns::PWM_MIN      = PWM_MIN;
#else
	uint16_t ns::PWM_MIN      = 1800;
#endif

#ifdef CONFIG_PWM_MAX
	uint16_t ns::PWM_MAX      = PWM_MAX;
#else
	uint16_t ns::PWM_MAX      = 3000;
#endif
float    ns::PCTRL_KP     = 0.002;
float    ns::PCTRL_KI     = 0.000001;
float    ns::PCTRL_KD     = 0.0002;
float    ns::SCTRL_KP     = 0.0;
float    ns::SCTRL_KI     = 0.0;
float    ns::SCTRL_KD     = 0.0;


#ifdef CONFIG_MOVE_MS_MIN
	int16_t  ns::MOVE_MS_MIN  = CONFIG_MOVE_MS_MIN;
#else
	int16_t  ns::MOVE_MS_MIN  = 1000;
#endif

#ifdef CONFIG_MOVE_MS_MAX
	int16_t  ns::MOVE_MS_MAX  = CONFIG_MOVE_MS_MAX;
#else
	int16_t  ns::MOVE_MS_MAX  = 3000;
#endif


static void load_defaults();
static bool try_parse(const char* s, double& out);
static bool try_parse(const char* s, float& out);

void ns::load(const char* file){
	load_defaults();
	BasePtr mb = Base::getInstance();
	SensorsPtr sen = Sensors::getInstance();

	mb->setDistPIDConstants(ns::PCTRL_KP, ns::PCTRL_KI, ns::PCTRL_KD);
	mb->setPWMCaps(ns::PWM_MAX, ns::PWM_MIN);
	mb->setMoveTimeLimits(ns::PWM_MAX, ns::PWM_MIN);

	// sen->setFloorSenOrder(ns::SEN_FLOOR);
	// sen->setDistSenAnglesDeg(ns::SEN_DIST);
	// sen->setLightSenAnglesDeg(ns::SEN_LIGHT);
}

static void load_defaults(){
	// Sensors
	#ifdef CONFIG_SEN_DIST
		// float    ns::SEN_DIST[]   = {CONFIG_SEN_DIST};
	#endif

	#ifdef CONFIG_SEN_LIGHT
		// float    ns::SEN_LIGHT[]  = {CONFIG_SEN_LIGHT};
	#endif

	#ifdef CONFIG_SEN_FLOOR
		// float    ns::SEN_FLOOR[]  = {CONFIG_SEN_FLOOR};
	#endif


	// Base control
	#ifdef CONFIG_PCTRL_KP
		try_parse(CONFIG_PCTRL_KP, ns::PCTRL_KP);
	#endif

	#ifdef CONFIG_PCTRL_KI
		try_parse(CONFIG_PCTRL_KI, ns::PCTRL_KI);
	#endif

	#ifdef CONFIG_PCTRL_KD
		try_parse(CONFIG_PCTRL_KD, ns::PCTRL_KD);
	#endif

	#ifdef CONFIG_SCTRL_KP
		try_parse(CONFIG_SCTRL_KP, ns::SCTRL_KP);
	#endif

	#ifdef CONFIG_SCTRL_KI
		try_parse(CONFIG_SCTRL_KI, ns::SCTRL_KI);
	#endif

	#ifdef CONFIG_SCTRL_KD
		try_parse(CONFIG_SCTRL_KD, ns::SCTRL_KD);
	#endif
}

static bool try_parse(const char* s, double& out){
	char *end;
	double d = strtod(s, &end);
	if( end == s ) return false;
	out = d;
	return true;
}

static bool try_parse(const char* s, float& out){
	char *end;
	double d = strtod(s, &end);
	if( end == s ) return false;
	out = d;
	return true;
}
