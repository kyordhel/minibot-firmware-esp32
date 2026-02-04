#include <esp_err.h>
#include <esp_log.h>

#include <lwip/sockets.h>

#include "constants.h"
#include "tcpserverbase.h"

static const char *TAG = "[TcpServer]";


namespace minibot::net{


void tcpServerTask(void* args);


TcpServerBase::TcpServerBase(uint16_t port, const char* taskName, uint8_t maxConn, uint16_t rcvBuffLen):
	port(port), taskName(taskName), rcvBuffLen(rcvBuffLen),
	sckt(INVALID_SOCKET), cliList(NULL), serverTaskHandle(NULL)
{
	cliList = new ClientList(maxConn);
	rcvBuff = (char*)malloc(rcvBuffLen);
}


TcpServerBase::~TcpServerBase(){
	dispose();
}


void TcpServerBase::start(){
	if( sckt != -1 ) return;
	if( !configureListener() ) return;
	if( !bindAndListen() ) return;
	ESP_LOGI(TAG, "Listening on %d", port);
	// Start async task
	xTaskCreate(tcpServerTask, taskName, 4096, this, 5, &serverTaskHandle);
}


bool TcpServerBase::configureListener(){
	int opt = 1;

	sckt = socket(AF_INET, SOCK_STREAM, IPPROTO_IP); // TCP IPv4
	if( sckt < 0 ) return false;

	setsockopt(sckt, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	int flags = fcntl(sckt, F_GETFL);
	fcntl(sckt, F_SETFL, flags | O_NONBLOCK);
	return true;
}


bool TcpServerBase::bindAndListen(){
	int res;

	struct sockaddr_in sockaddr;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen any IP address on adapter
	sockaddr.sin_family      = AF_INET;
	sockaddr.sin_port        = htons(port);

	res = bind(sckt, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
	if (res != 0) {
		ESP_LOGE(TAG, "Unable to bind to address: errno %d", errno);
		dispose();
		return false;
	}

	res = listen(sckt, 1);
	if (res != 0) {
		ESP_LOGE(TAG, "Socket failed to listen");
		dispose();
		return false;
	}
	return true;
}


void TcpServerBase::serverTask(){
	while(true){
		acceptClients();
		if( cliList->empty() ){
			vTaskDelay( pdMS_TO_TICKS(100) );
			continue;
		}
		serveClients();
	}
}

void TcpServerBase::acceptClients(){
	if( (sckt < 0) || cliList->full() ) return;
	struct sockaddr_storage source_addr;
	socklen_t addr_len;
	int cli = accept(sckt, (sockaddr*)&source_addr, &addr_len);

	// New connection accepted
	if(cli >= 0){
		cliList->add(cli);
		int flags = fcntl(cli, F_GETFL);
		fcntl(cli, F_SETFL, flags | O_NONBLOCK);
		// Set socket as non-blocking
	}

	// Error while accepting connection.
	else if( (cli < 0) && (errno != EWOULDBLOCK) )
		dispose();
}

void TcpServerBase::diconnectClient(const int sckt){
	close(sckt);
	cliList->remove(sckt);
}

void TcpServerBase::diconnectClientByIndex(uint8_t ix){
	close(cliList->get(ix));
	cliList->removeAt(ix);
}


void TcpServerBase::serveClients(){
	int count;
	if (sckt < 0) return;
	for(uint8_t ix = 0; ix < cliList->getCount(); ++ix){
		// Skip no socket
		if( cliList->get(ix) == INVALID_SOCKET ) continue;
		// Attempt to receive, disconnect client on error and remove
		count = recv( cliList->get(ix), rcvBuff, rcvBuffLen, 0 );
		if( count > 0 )
			handleRcvData( cliList->get(ix), rcvBuff, count );
		else if (count == 0 ){ // Disconnected
			diconnectClientByIndex(ix);
		}
		else if (errno == ENOTCONN ) // Disconnected
			cliList->removeAt(ix);
		else if( (errno == EINPROGRESS) || (errno == EAGAIN) || (errno == EWOULDBLOCK) ) // No data received
			continue;
		else{
			// Reached this point there was an unexpected error.
			// Close the connection and remove the client.
			diconnectClientByIndex(ix);
		}
		vTaskDelay( pdMS_TO_TICKS(1) );
	}
}


void TcpServerBase::dispose(){
	if(sckt >= 0) close(sckt);
	sckt = INVALID_SOCKET;
	if(serverTaskHandle) vTaskDelete(serverTaskHandle);
}


bool TcpServerBase::sendTo(const int sckt, const void* data, size_t length){
	int sent = 0;
	const uint8_t* uidata = (const uint8_t*)data;
	size_t pending = length;
	while( pending > 0){
		sent = send(sckt, uidata + (length - pending), pending, 0);
		if( (sent < 0) && (errno != EINPROGRESS) && (errno != EAGAIN) && (errno != EWOULDBLOCK) )
			return false;
		ESP_LOGI(TAG, "sendTo %lu bytes sent", sent);
		pending-= sent;
	}
	return true;
}


void tcpServerTask(void* args){
	TcpServerBase* server = (TcpServerBase*)args;
	if( !server ){
		ESP_LOGI(TAG, "ERROR");
		vTaskDelete(NULL);
		return;
	}
	server->serverTask();
}

}