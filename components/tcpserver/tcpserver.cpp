#include "tcpserver.h"
#include "tcpserverbase.h"

#include <cstdlib>
#include <cstring>


#include <esp_err.h>
#include <esp_log.h>

namespace minibot::tcpserver{

static const char* TAG = "[SRV]";


class TcpServer : public minibot::net::TcpServerBase{
public:
	TcpServer(uint16_t port);
private:
	TcpServer(const TcpServer&)            = delete;
	TcpServer& operator=(const TcpServer&) = delete;

public:
	void setCommandHandler(commandHandler_t);

protected:
	virtual void handleRcvData(int sckt, char* rcvBuff, uint16_t count);
	virtual bool sendTo(const int sckt, const reply_t& rply);

protected:
	void handleRequest(int sckt, const request_t& req);

protected:
	commandHandler_t handleCommand;
};



static TcpServer* server = NULL;

void start(uint16_t port, commandHandler_t cmdh){
	if(server != NULL) return;
	server = new TcpServer(port);
	server->setCommandHandler(cmdh);
	server->start();
}


using minibot::net::TcpServerBase;

TcpServer::TcpServer(uint16_t port) : TcpServerBase(port, "TcpServer", 5, 1024), handleCommand(NULL) {}

void TcpServer::setCommandHandler(commandHandler_t cmdH){
	handleCommand = cmdH;
}

void TcpServer::handleRcvData(int sckt, char* rcvBuff, uint16_t count){
	uint32_t msize, esize, cmdId;
	Command cmd;
	request_t req;
	ESP_LOGI(TAG, "Received %u bytes: %s", count, rcvBuff);

	if( count < 12 ) return; // Discard messages smaller than 12 bytes
	// 1. Read Packet size, Command Id and Command (32bit each)
	msize = *((uint32_t*)rcvBuff);
	cmdId = *((uint32_t*)(rcvBuff+4));
	cmd   = *((Command*)(rcvBuff+8));
	ESP_LOGI(TAG, "msize: 0x%08x %u", msize, msize);
	ESP_LOGI(TAG, "cmdId: 0x%08x %u", cmdId, cmdId);
	ESP_LOGI(TAG, "cmd:   0x%08x %u", cmd,   cmd);

	if( count != msize ) return; // Size missmatch or malformed

	// 2. Check command and size matches
	switch(cmd){
		// Zero args commands, 12 bytes
		case Command::STOP:      case Command::GetPWM:    case Command::GetSpeed:
		case Command::ReadAll:   case Command::ReadBatt:  case Command::ReadDist:
		case Command::ReadFloor: case Command::ReadLight:
			esize = 12;
			break;
		// Two floats commands, 20 bytes
		case Command::MV:        case Command::SetPWM2:   case Command::SetSpeed2:
			esize = 12 + 2*sizeof(float);
			break;
		// Four floats commands, 28 bytes
		case Command::SetPWM4: case Command::SetSpeed4:
			esize = 12 + 4*sizeof(float);
			break;

		default:
			return;
	}

	if( esize != msize ){
		ESP_LOGI(TAG, "Size mismatch");
		return; // Size missmatch
	}

	// 3. Assembly request
	std::memcpy(&req, rcvBuff, msize);

	// 4. Handle the request
	handleRequest(sckt, req);
}



void TcpServer::handleRequest(int sckt, const request_t& req){
	// 1. Generate default (failure) response
	reply_t rpl = reply_from_request(req, false);

	// 2. Queries are performed in-situ, movements are transferred to behaviors.
	ESP_LOGI(TAG, "handleCommand: %p", handleCommand);
	ESP_LOGI(TAG, "req.size: 0x%08x", req.size);
	ESP_LOGI(TAG, "req.id:   0x%08x", req.id);
	ESP_LOGI(TAG, "req.cmd:  0x%08x", req.cmd);
	if(handleCommand && !handleCommand(req, rpl))
		rpl.success = false;

	// 3. Send reply
	ESP_LOGI(TAG, "Command: %s", rpl.success? "Success" : "Failed");
	ESP_LOGI(TAG, "RPL: 0x%020x", &rpl);

	rpl.size = get_reply_payload_size(rpl);
	ESP_LOGI(TAG, "rpl.size: 0x%08x", rpl.size);
	ESP_LOGI(TAG, "rpl.id:   0x%08x", rpl.id);
	ESP_LOGI(TAG, "rpl.cmd:  0x%08x", rpl.cmd);
	sendTo(sckt, rpl);
}


bool TcpServer::sendTo(const int sckt, const reply_t& rply){
	return TcpServerBase::sendTo(sckt, (uint8_t*)&rply, rply.size);
}

} // End namespace