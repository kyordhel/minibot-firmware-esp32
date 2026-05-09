#include <cmath>
#include <cstdio>
#include <sdkconfig.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_event.h>
#include <esp_wifi.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include "base.h"
#include "sensors.h"
#include "wifi.h"
#include "storage.h"
#include "settings.h"
#include "tcpserver.h"
#include "webserver.h"
#include "behaviors.h"

static const char *TAG = "[Minibot]";


using namespace minibot;

BasePtr mb = NULL;
SensorsPtr sen = NULL;
volatile int8_t behavior = 0;

void execute_behavior();
bool tcp_server_command_handler(const minibot::tcpserver::request_t& req, minibot::tcpserver::reply_t& rply);
bool web_server_command_handler(const minibot::webserver::request_t& req, minibot::webserver::reply_t& rply);

// app_main requires C linkage.
extern "C"{
	void app_main(void);
}



void app_main(void){
	// vTaskDelay( pdMS_TO_TICKS(1000) );

	I2CDriverPtr i2c = I2CDriver::makeDefault(I2C_NUM_0, (gpio_num_t)CONFIG_I2C_SDA, (gpio_num_t)CONFIG_I2C_SCL);
	sen = Sensors::getInstance(i2c);
	mb = Base::getInstance(i2c);

	ESP_LOGI(TAG, "Minibot v0.1 initialized");
	// mb->readBattVolt();
	// ESP_LOGI(TAG, "Battery:  %0.1fV (%0.1f%%)", mb->readBattVolt(), mb->readBattPerc());

	// sdistr_t  dist;
	// sfloorr_t floor;
	// slightr_t light;
	// uint8_t values[DIST_SENSOR_COUNT];

	// while(1){
		// i2c->detect();
		// printf("\n\n");
		// vTaskDelay( pdMS_TO_TICKS(1000) );
		// continue;

		// printf("\n\nLight:");
		// sen->readLight(light);
		// for(uint8_t i = 0; i < 8; ++i) printf(" %0.2f", light.data[i].value);

		// printf("\nFloor:");
		// sen->readFloor(floor);
		// for(uint8_t i = 0; i < 4; ++i) printf(" %0.2f", floor.data[i]);

		// sen->readDistance(dist);
		// printf("\nDist (%u):", dist.count);
		// for(uint8_t i = 0; i < dist.count; ++i) printf(" (%0.0f, %0.2f)", dist.data[i].angle * 180 / 3.141592, dist.data[i].value);

		// sen->readDistanceRaw(values);
		// printf("\nDist:");
		// for(uint8_t i = 0; i < DIST_SENSOR_COUNT; ++i) printf(" % 3d", values[i]);

		// vTaskDelay( pdMS_TO_TICKS(1000) );
	// }

	// mb->mv(0.1, 1);

	// settings::load("settings.conf");
	// ESP_LOGI(TAG, "Settings loaded");
	storage::init();

	setup_wifi();
	ESP_LOGI(TAG, "Wifi setup complete");

	// tcpserver::start(9000, &tcp_server_command_handler);
	// ESP_LOGI(TAG, "Tcp server started");

	webserver::start(&web_server_command_handler);
	ESP_LOGI(TAG, "Web server started");

	// mb.mv(1, 2);
	ESP_LOGI(TAG, "Polling");
	while(true){
		execute_behavior();
	}
}


void execute_behavior(){
	switch(behavior){
		case 0:
			behavior0_step();
			break;

		case 1:
			behavior1_step();
			break;

		default:
			vTaskDelay( pdMS_TO_TICKS(1000) );
			break;
	}
}


bool tcp_server_command_handler(const minibot::tcpserver::request_t& req, minibot::tcpserver::reply_t& rply){
	float f0, f1, f2, f3;
	using namespace minibot::tcpserver;
	ESP_LOGI(TAG, "handleCommand 0");
	if( !mb || !sen) return false;

	// Queries are performed in-situ, movements are transferred to behaviors.
	ESP_LOGI(TAG, "handleCommand 1 | req.cmd: 0x%08x", req.cmd);
	switch(req.cmd){
		case Command::STOP:
			mb->stop();
			rply.success = true;
			return true;

		case Command::MV:
			float d, a;
			mb->mv(req.args.fargs[0], req.args.fargs[1], d, a);
			rply.args.fargs[0] = d * cos(a + M_PI_2);
			rply.args.fargs[1] = d * sin(a + M_PI_2);
			rply.args.fargs[2] = a;
			rply.success = true;
			return true;

		case Command::GetPWM:
			mb->getPwm(f0, f1, f2, f3);
			rply.args.fargs[0] = f0;  rply.args.fargs[1] = f1;
			rply.args.fargs[2] = f2;  rply.args.fargs[3] = f3;
			rply.success = true;
			return true;

		case Command::GetSpeed:
			mb->getSpeed(f0, f1, f2, f3);
			rply.args.fargs[0] = f0;  rply.args.fargs[1] = f1;
			rply.args.fargs[2] = f2;  rply.args.fargs[3] = f3;
			rply.success = true;
			return true;

		case Command::ReadAll:
			sen->readDistance(rply.args.dist);
			sen->readFloor(rply.args.floor);
			sen->readLight(rply.args.light);
			rply.args.batt.voltage = mb->readBattVolt();
			rply.args.batt.percentage = mb->readBattPerc();
			rply.success = true;
			return true;

		case Command::ReadBatt:
			rply.args.batt.voltage = mb->readBattVolt();
			rply.args.batt.percentage = mb->readBattPerc();
			rply.success = true;
			return true;

		case Command::ReadDist:
			// return sen->readDistance(rply.args.dist);
			sen->readDistance(rply.args.dist);
			rply.success = true;
			return true;

		case Command::ReadFloor:
			// return sen->readFloor(rply.args.floor);
			sen->readFloor(rply.args.floor);
			rply.success = true;
			return true;

		case Command::ReadLight:
			// return sen->readLight(rply.args.light);
			sen->readLight(rply.args.light);
			rply.success = true;
			return true;

		case Command::SetPWM2:
			// return mb->setPwm(req.args.fargs[0], req.args.fargs[1]);
			mb->setPwm(req.args.fargs[0], req.args.fargs[1]);
			rply.success = true;
			return true;

		case Command::SetSpeed2:
			// return mb->setSpeed(req.args.fargs[0], req.args.fargs[1]);
			mb->setSpeed(req.args.fargs[0], req.args.fargs[1]);
			rply.success = true;
			return true;

		case Command::SetPWM4:
			// return mb->setPwm(req.args.fargs[0], req.args.fargs[1], req.args.fargs[2], req.args.fargs[3]);
			mb->setPwm(req.args.fargs[0], req.args.fargs[1], req.args.fargs[2], req.args.fargs[3]);
			rply.success = true;
			return true;

		case Command::SetSpeed4:
			// return mb->setSpeed(req.args.fargs[0], req.args.fargs[1], req.args.fargs[2], req.args.fargs[3]);
			mb->setSpeed(req.args.fargs[0], req.args.fargs[1], req.args.fargs[2], req.args.fargs[3]);
			rply.success = true;
			return true;

		default:
			ESP_LOGI(TAG, "handleCommand -1");
			return false;
	}

	ESP_LOGI(TAG, "handleCommand -2");
	return false;
}


bool web_server_command_handler(const minibot::webserver::request_t& req, minibot::webserver::reply_t& rply){
	if( !mb || !sen) return false;
	// ESP_LOGI(TAG, "web_server_command_handler 1");
	// ESP_LOGI(TAG, "req.cmd=%s | req.nfargs=%d | req.fargs={%f, %f, %f, %f} ", req.cmd, req.nfargs, req.fargs[0], req.fargs[1], req.fargs[2], req.fargs[3]);

	if( !strcmp(req.cmd, "stop") ){
		mb->stop();
		return true;
	}
	else if( !strcmp(req.cmd, "mv") ){
		ESP_LOGI(TAG, "req.cmd=%s | req.nfargs=%d | req.fargs={%f, %f} ", req.cmd, req.nfargs, req.fargs[0], req.fargs[1]);
		mb->mv(req.fargs[0], req.fargs[1], rply.fargs[0], rply.fargs[1]);
		ESP_LOGI(TAG, "rply.fargs={%f, %f} ", rply.fargs[0], rply.fargs[1]);
		return true;
	}
	else if( !strcmp(req.cmd, "speed") ){
	// ESP_LOGI(TAG, "web_server_command_handler 2");
		if( req.nfargs == 2 )
			mb->setPwm(req.fargs[0], rply.fargs[1]);
		else if( req.nfargs == 4 )
			mb->setPwm(req.fargs[0], req.fargs[1], req.fargs[2], req.fargs[3]);
		// ESP_LOGI(TAG, "web_server_command_handler 3");
		mb->getPwm(rply.fargs[0], rply.fargs[1], rply.fargs[2], rply.fargs[3]);
		// ESP_LOGI(TAG, "web_server_command_handler 4");
		return true;
	}
	else if( !strcmp(req.cmd, "pwm") ){
	// ESP_LOGI(TAG, "web_server_command_handler 2");
		if( req.nfargs == 2 )
			mb->setSpeed(req.fargs[0], rply.fargs[1]);
		else if( req.nfargs == 4 )
			mb->setSpeed(rply.fargs[0], rply.fargs[1], rply.fargs[2], rply.fargs[3]);
		// ESP_LOGI(TAG, "web_server_command_handler 3");
		mb->getSpeed(rply.fargs[0], rply.fargs[1], rply.fargs[2], rply.fargs[3]);
		// ESP_LOGI(TAG, "web_server_command_handler 4");
		return true;
	}
	else if( !strcmp(req.cmd, "behavior") ){
		if( req.nfargs == 1 ) behavior = req.fargs[0];
		rply.fargs[0] = behavior;
		return true;
	}
	else if( !strcmp(req.cmd, "sensors") ){
	// ESP_LOGI(TAG, "web_server_command_handler 2");
		sen->readDistance(rply.all.dist);
		// ESP_LOGI(TAG, "web_server_command_handler 3");
		sen->readFloor(rply.all.floor);
		// ESP_LOGI(TAG, "web_server_command_handler 4");
		sen->readLight(rply.all.light);
		// ESP_LOGI(TAG, "web_server_command_handler 5");
		mb->readBatt(rply.all.batt);
		// ESP_LOGI(TAG, "web_server_command_handler 6");
		return true;
	}
	return false;
}