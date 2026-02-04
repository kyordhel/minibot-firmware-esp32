#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "clientlist.h"

namespace minibot::net{

class TcpServerBase{
public:
	TcpServerBase(uint16_t port, const char* taskName, uint8_t maxConn, uint16_t rcvBuffLen);
	~TcpServerBase();

private:
	TcpServerBase(const TcpServerBase&)            = delete;
	TcpServerBase& operator=(const TcpServerBase&) = delete;

public:
	void start();

protected:
	void serverTask();

protected:
	bool configureListener();
	bool bindAndListen();
	void acceptClients();
	void serveClients();
	void diconnectClient(const int sckt);
	void diconnectClientByIndex(uint8_t ix);
	void dispose();
	bool sendTo(const int sckt, const void* data, size_t length);

protected:
	virtual void handleRcvData(int sckt, char* rcvBuff, uint16_t count) = 0;

protected:
	friend void tcpServerTask(void* args);

protected:
	uint16_t port;
	const char* taskName;
	const uint16_t rcvBuffLen;
	char* rcvBuff;

private:
	int sckt;
	ClientList* cliList;
	TaskHandle_t serverTaskHandle;
};


} // End namespace
