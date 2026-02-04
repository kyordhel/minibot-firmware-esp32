#include "webserver.h"
#include "tcpserverbase.h"
#include "helpers.h"
#include "storage.h"

#include <cstdlib>
#include <cstring>

#include <esp_err.h>
#include <esp_log.h>

namespace minibot::webserver{

static const char* TAG = "[WEB]";


class WebServer : public minibot::net::TcpServerBase{
public:
	WebServer();
private:
	WebServer(const WebServer&)            = delete;
	WebServer& operator=(const WebServer&) = delete;

public:
	void setCommandHandler(commandHandler_t);

protected:
	bool fetchRequest(const char* rcvBuff, uint16_t rcvBuffLen, char* reqstr);
	bool serveRequest(int sckt, const char* reqstr);
	bool serveFile(int sckt, const char* reqstr);
	void sendResult(int sckt, const request_t& req, const reply_t& rply, bool result);
	void send404(int sckt, const char* file);
	void send500(int sckt, const char* err);
	void sendErr(int sckt, uint16_t error_code, const char* error_code_label=NULL, const char* message=NULL);
	bool handleCommandRequest(int sckt, const char* reqstr);

protected:
	virtual void handleRcvData(int sckt, char* rcvBuff, uint16_t count);

protected:
	commandHandler_t handleCommand;
	// 1380bytes is the max payload for any modern TCP package
	// 1380bytes + 3bytes len + 4bytes crlf + NULL + pad
	char txBuff[1400];

	// Following variables kept here to reduce stack size in task
	char reqstr[256];   // Stores the current request (handleRcvData)
	char filepath[255]; // Absolute path of the file being served (serveFile)
};



static WebServer* server = NULL;
static inline char decode(char** cc);
static inline uint8_t hex2dec(char c);

void start(commandHandler_t cmdh){
	if(server != NULL) return;
	server = new WebServer();
	server->setCommandHandler(cmdh);
	server->start();
}


WebServer::WebServer() : TcpServerBase(80, "WebServer", 5, 1024),
	handleCommand(NULL){}

void WebServer::setCommandHandler(commandHandler_t cmdH){
	handleCommand = cmdH;
}

void WebServer::handleRcvData(int sckt, char* rcvBuff, uint16_t count){
	if( count < 10 ) return;

	if( strncmp(rcvBuff, "GET ", 4) != 0 ) return;
	if( !fetchRequest(rcvBuff, count, reqstr) ) return;

	serveRequest(sckt, reqstr);
	rcvBuff[0] = 0;
	diconnectClient(sckt);
}

bool WebServer::fetchRequest(const char* rcvBuff, uint16_t rcvBuffLen, char* reqstr){
	uint16_t cc = 4, i = 0;
	reqstr[0] = 0;
	while( (cc < rcvBuffLen) && (rcvBuff[cc] != 0x20) )
		reqstr[i++] = rcvBuff[cc++];
	if( cc == 4 ){
		reqstr[0] = 0;
		return false;
	}
	reqstr[i] = 0;
	return true;
}

bool WebServer::serveRequest(int sckt, const char* reqstr){
	if( !strncmp(reqstr, "/minibot?cmd=", 13) )
		return handleCommandRequest(sckt, reqstr);

	else if(
			(reqstr[0] == 0) || !strcmp(reqstr, "/")  ||
			!strncmp(reqstr, "/index.htm", 10) )
		return serveFile(sckt, "index.htm");
	return serveFile(sckt, reqstr + 1);
}

bool WebServer::serveFile(int sckt, const char* file){
	minibot::storage::realpath(filepath, file);
	FILE* fp = fopen(filepath, "rb");

	// 1. If file does not exist, send 404
	if( fp == NULL ){
		ESP_LOGE(TAG, "Failed to open file '%s'", file);
		send404(sckt, file);
		return false;
	}

	// 2. Get mime from extension
	const char* mime = mime_from_ext(ext_from_file(file));
	// 3. Send header
	write_ok_header(txBuff, sizeof(txBuff), mime);
	sendTo(sckt, txBuff, strlen(txBuff));

	// 4. Send file in chunks
	fseek(fp, 0, SEEK_END);
	size_t fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	size_t toRead = 0;
	size_t preSize = 0;
	size_t bytesRead = 0;


	for(size_t pending = fileSize; pending > 0; pending-= bytesRead){
		toRead = (pending < 1380) ? pending : 1380;
		sprintf(txBuff, "%x\r\n", toRead);
		preSize = strlen(txBuff);
		bytesRead = fread(txBuff + preSize, 1, toRead, fp);
		sprintf(txBuff + preSize + bytesRead, "\r\n");
		if(!sendTo(sckt, txBuff, preSize + bytesRead + 2))
			break;
	}
	sprintf(txBuff, "0\r\n\r\n");
	sendTo(sckt, txBuff, 5);

	ESP_LOGI(TAG, "Served file: %s (%lubytes)", filepath, fileSize);
	fclose(fp);
	return true;
}

bool WebServer::handleCommandRequest(int sckt, const char* reqstr){
	bool res;
	request_t req;
	reply_t rply;

	memset(&req,  0, sizeof(req));
	memset(&rply, 0, sizeof(rply));
	ESP_LOGI(TAG, "handleCommandRequest 0");
	// "/minibot?cmd=<command>&args=<space-separated-args>"

	// 1. Retrieve command
	for(uint8_t i=13, j=0; j < sizeof(req.cmd)-1; ++i, ++j){
		if( (reqstr[i] < 'a') || (reqstr[i] > 'z') ) break;
		req.cmd[j] = reqstr[i];
		req.cmd[j+1] = 0;
	}

	ESP_LOGI(TAG, "handleCommandRequest 1");
	// 2. Retrieve arguments
	char* cc = (char*)strstr(reqstr, "&args=");
	if(cc){
		cc+=6;
		ESP_LOGI(TAG, "cc=%s", cc);
		if(cc != NULL){
			for(uint8_t i = 0; (*cc != 0) && (i < 64); ++i, ++cc)
				req.sargs[i] = decode(&cc);
			req.nfargs = sscanf(req.sargs, "%f %f %f %f", &req.fargs[0], &req.fargs[1], &req.fargs[2], &req.fargs[3]);
		}
	}

	ESP_LOGI(TAG, "handleCommandRequest 2");
	ESP_LOGI(TAG, "req.cmd=%s | req.sargs=%s\n", req.cmd, req.sargs);
	if( !handleCommand ){ // No handle, execution automatically fails
		sendResult(sckt, req, rply, false);
		// send500(sckt, "Unimplemented. WebServer::handleCommand is NULL");
		return false;
	}

	ESP_LOGI(TAG, "handleCommandRequest 3");
	// Command is any of {stop, mv, speed, pwm, behavior, sensors}
	res = handleCommand(req, rply);
	ESP_LOGI(TAG, "handleCommandRequest 4");
	sendResult(sckt, req, rply, res);
	return res;
}


void WebServer::sendResult(int sckt, const request_t& req, const reply_t& rply, bool result){
	ESP_LOGI(TAG, "sendResult");

	write_ok_header(txBuff, sizeof(txBuff), "application/json", 0, false);
	char* cc = txBuff + strlen(txBuff);
	char* bcc = cc;

	if(strlen(req.sargs) > 0)
		sprintf(cc, "{\"req\":\"%s %s\",\"res\":", req.cmd, req.sargs);
	else
		sprintf(cc, "{\"req\":\"%s\",\"res\":", req.cmd);
	cc = txBuff + strlen(txBuff);
	ESP_LOGI(TAG, "sendResult | bcc=%s", bcc);
	if(!result)
		sprintf(cc, "null}\r\n");
	else if( !strcmp(req.cmd, "stop") )
		sprintf(cc, "\"OK\"}\r\n");
	else if( !strcmp(req.cmd, "mv") )
		sprintf(cc, "[%0.3f, %0.3f]}\r\n", rply.fargs[0], rply.fargs[1]);
		// sprintf(cc, "[%0.3f, %0.3f, %0.3f]}\r\n", rply.fargs[0], rply.fargs[1], rply.fargs[2]);
	else if( !strcmp(req.cmd, "speed") )
		sprintf(cc, "[%0.4f, %0.4f, %0.4f, %0.4f]}\r\n", rply.fargs[0], rply.fargs[1], rply.fargs[2], rply.fargs[3]);
	else if( !strcmp(req.cmd, "pwm") )
		sprintf(cc, "[%0.4f, %0.4f, %0.4f, %0.4f]}\r\n", rply.fargs[0], rply.fargs[1], rply.fargs[2], rply.fargs[3]);
	else if( !strcmp(req.cmd, "behavior") )
		sprintf(cc, "%ld}\r\n", rply.iargs[0]);
	else if( !strcmp(req.cmd, "sensors") ){
		// ESP_LOGI(TAG, "sendResult | sensors->light | bcc=%s", bcc);
		sprintf(cc, "{\"light\":[");
		for(uint8_t i = 0; i < LIGHT_SENSOR_COUNT; ++i){
			cc+=strlen(cc);
			const char* sep = (i < LIGHT_SENSOR_COUNT-1) ? "," : "";
			sprintf(cc, "[%0.8f,%0.4f]%s", rply.all.light.data[i].angle, rply.all.light.data[i].value, sep);
		}
		cc+=strlen(cc);
		// ESP_LOGI(TAG, "sendResult | sensors->distance | bcc=%s", bcc);
		sprintf(cc, "],\"distance\":[");
		for(uint8_t i = 0; i < DIST_SENSOR_COUNT; ++i){
			cc+=strlen(cc);
			const char* sep = (i < DIST_SENSOR_COUNT-1) ? "," : "";
			sprintf(cc, "[%0.8f,%0.4f]%s", rply.all.dist.data[i].angle, rply.all.dist.data[i].value, sep);
		}
		cc+=strlen(cc);
		// ESP_LOGI(TAG, "sendResult | sensors->battery | bcc=%s", bcc);
		sprintf(cc, "],\"battery\":%d}}\r\n", rply.all.batt.percentage);
	}
	else
		sprintf(cc, "\"%s\"}\r\n", rply.cargs);
	ESP_LOGI(TAG, "sendResult | bcc=%s", bcc);
	sendTo(sckt, txBuff, strlen(txBuff));

}

void WebServer::send404(int sckt, const char* file){
	char msg[256];
	sprintf(msg, "<p>The requested file,<br><pre>%s</pre><br>could not be found in the server.</p>", file);
	sendErr(sckt, 404, "Not Found", msg);
}

void WebServer::send500(int sckt, const char* err){
	char msg[256];
	sprintf(msg, "<p>An unexpected error was found when processing the request<br><pre>%s</pre>", err);
	sendErr(sckt, 500, "Internal Server Error");
}

void WebServer::sendErr(int sckt, uint16_t error_code, const char* error_code_label, const char* message){
	write_error_header(txBuff, 1024, error_code, error_code_label, message);
	sendTo(sckt, txBuff, strlen(txBuff));
	ESP_LOGI(TAG, "Served error %d: %s (%lubytes)", error_code, error_code_label, strlen(txBuff));
}


static inline
char decode(char** cc){
	if(**cc == '%'){
		if( (*(*cc+1) == 0) || (*(*cc+2) == 0) ) return 0;
		*cc+=1;
		char c = hex2dec(**cc) * 16;
		*cc+=1;
		c+= hex2dec(**cc);
		return c;
	}
	return **cc;
}

static inline
uint8_t hex2dec(char c){
	if((c >= '0') && (c <= '9')) return c-'0';
	else if((c >= 'A') && (c <= 'F')) return c-'A';
	else if((c >= 'a') && (c <= 'f')) return c-'a';
	return 0;
}

} // End namespace
