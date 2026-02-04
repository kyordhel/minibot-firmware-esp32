#pragma once

#include <memory>

#include "request.h"
#include "hal.h"

namespace minibot::tcpserver{


struct __attribute__((packed)) {
	uint32_t size;
	uint32_t id;
	Command  cmd;
	uint8_t  success;
	// Worst case scenario is READ_ALL:
	// f+b battery, 4×ff floor, 8×ff light, H+16×ff dist = 231 bytes
	union{
		float         fargs[64];
		uint32_t      Iargs[64];
		int32_t       iargs[64];
		uint16_t      Hargs[128];
		int16_t       hargs[128];
		uint8_t       Bargs[256];
		int8_t        bargs[256];
		char          cargs[256];
		sfloorr_t     floor;
		slightr_t     light;
		sdistr_t      dist;
		sbatteryr_t   batt;
		sall_t        all;
	} args;
} typedef reply_t;

reply_t reply_from_request(const request_t& req, bool success=false);

uint16_t get_reply_payload_size(const reply_t& rpl);


};
