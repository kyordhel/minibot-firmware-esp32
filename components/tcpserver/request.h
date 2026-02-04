#pragma once

#include <memory>

namespace minibot::tcpserver{

// Commands encoded in little endian
enum class Command : uint32_t {
	MV          = 0x0100,
	GetPWM      = 0x1000,
	SetPWM2     = 0x1200,
	SetPWM4     = 0x1400,
	GetSpeed    = 0x2000,
	SetSpeed2   = 0x2200,
	SetSpeed4   = 0x2400,
	ReadAll     = 0x4000,
	ReadBatt    = 0x4100,
	ReadDist    = 0x4200,
	ReadFloor   = 0x4300,
	ReadLight   = 0x4400,
	STOP        = 0x0000,

	S_MV        = 0x2020766d, // Maps to "mv  "
	S_PWM       = 0x206d7770, // Maps to "pwm "
	S_Speed     = 0x20647073, // Maps to "spd "
	S_ReadAll   = 0x20617372, // Maps to "rsa "
	S_ReadBatt  = 0x20627372, // Maps to "rsb "
	S_ReadDist  = 0x20647372, // Maps to "rsd "
	S_ReadLight = 0x206c7372, // Maps to "rsl "
	S_ReadFloor = 0x20667372, // Maps to "rsf "

	Undefined   = 0xffffffff,
};


struct __attribute__((packed)) {
	uint32_t size;
	uint32_t id;
	Command  cmd;
	union{
		float    fargs[4];
		uint32_t uargs[4];
		int32_t  iargs[4];
	} args;
} typedef request_t;

};
