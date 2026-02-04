#pragma once

#include "hal.h"
#include <cstdint>
#include <cstdlib>

namespace minibot::webserver{

struct{
	char cmd[16];
	char sargs[64];
	int8_t nfargs;
	float fargs[4];
} typedef request_t;

union{
	float         fargs[4];
	int32_t       iargs[4];
	char          cargs[128];
	sfloorr_t     floor;
	slightr_t     light;
	sdistr_t      dist;
	sbatteryr_t   batt;
	sall_t        all;
} typedef reply_t;

typedef bool (*commandHandler_t)(const request_t& req, reply_t& rply);

void start(commandHandler_t cmdh=NULL);

}// End namespace