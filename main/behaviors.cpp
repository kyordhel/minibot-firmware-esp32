#include "behaviors.h"
#include "minibot.h"

#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

using namespace minibot;

const float ADVANCE = 0.1; // How much the robot advances every step
const float OBS_DST = 0.1; // Distance to obstacle to trigger avoidance


enum class Obstacle : uint8_t{
	None  = 0x00,
	Left  = 0x02,
	Right = 0x01,
	Front = 0x03,
};

const char OBS_NAMES[4][8] = {"none", "left", "right", "front"};

const char* get_obstacle_name(Obstacle obs);
Obstacle detect_obstacles();
void avoid_obstacle(Obstacle obs);
void move_towards_light(float ls_angle);


void behavior0_step(){
	// Base::getInstance()->stop();
	vTaskDelay( pdMS_TO_TICKS(1000) );
}


void behavior1_step(){
	float ls_angle, ls_strength;
	get_light_source(ls_angle, ls_strength);
	if (ls_strength < 0.05){ // Arrived to light source
		printf("Arrived to light source (r=%0.3f, θ=%0.1f)", ls_strength, ls_angle * 57.3f );
		vTaskDelay( pdMS_TO_TICKS(1000) );
		return;
	}
	printf("Light source detected: %s (θ=%0.1f)", get_light_quadrant_name(ls_angle), ls_angle*57.3f );
	Obstacle obs = detect_obstacles();
	printf("  Obstacles? %s", get_obstacle_name(obs));
	if(obs == Obstacle::None) move_towards_light(ls_angle);
	else avoid_obstacle(obs);
}


const char* get_obstacle_name(Obstacle obs){
	return OBS_NAMES[(uint8_t)obs];
}


Obstacle detect_obstacles(){
	sdistr_t reading;
	Sensors::getInstance()->readDistance(reading);
	bool obsl = ( reading.data[0].value > 0 ) && ( (reading.data[0].value * 0.5) <= OBS_DST );
	bool obsf = ( reading.data[1].value > 0 ) && ( (reading.data[1].value * 1.0) <= OBS_DST );
	bool obsr = ( reading.data[2].value > 0 ) && ( (reading.data[2].value * 0.5) <= OBS_DST );
	if (obsf || (obsl && obsr)) return Obstacle::Front;
	else if( obsl ) return Obstacle::Left;
	else if( obsr ) return Obstacle::Right;
	return Obstacle::None;
}


void avoid_obstacle(Obstacle obs){
	BasePtr mb = Base::getInstance();
	switch(obs){
		case Obstacle::Left:
			mb->mv(ADVANCE, -PI_4);
			break;

		case Obstacle::Right:
			mb->mv(ADVANCE,  PI_4);
			break;

		case Obstacle::Front:
			mb->mv(ADVANCE,  PI_2);
			mb->mv(      0, -PI_2);
			break;

		default: return;
	}
}


void move_towards_light(float ls_angle){
	BasePtr mb = Base::getInstance();
	// Robot advances over y axis (90° means no rotation).
	// Thus angle must be shifted -90°
	// 1. Substract π/2 from angle
	float ra = ls_angle - PI_2;
	// 2. Whatever the angle is, normalize it to [0, 2π)
	while(ra < 0)         ra+= TWO_PI;
	while(ra >= TWO_PI) ra-= TWO_PI;
	// 3. Shift back to [-π, π] for the shortest turn
	if(ra > PI) ra-= TWO_PI;
	mb->mv(0.1, ra);
}

