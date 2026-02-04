#pragma once

#include<cstdint>

#include "request.h"
#include "reply.h"

namespace minibot::tcpserver{

typedef bool (*commandHandler_t)(const request_t& req, reply_t& rply);

void start(uint16_t port, commandHandler_t cmdh);

}// End namespace