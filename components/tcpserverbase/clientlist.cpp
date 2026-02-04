#include "clientlist.h"
#include "constants.h"

#include <cstdlib>

using namespace minibot::net;

ClientList::ClientList(uint8_t size):
	size(size), count(0), nfsix(0), clients(new int[size])
{
	for(uint8_t ix = 0; ix < size; ++ix)
		clients[ix] = INVALID_SOCKET;
}


ClientList::~ClientList(){
	size = 0;
	for(uint8_t ix = 0; ix < size; ++ix)
		clients[ix] = INVALID_SOCKET;
	delete clients;
	clients = NULL;
}


bool ClientList::add(int socket){
	if( count >= size ) return false;
	clients[nfsix] = socket;
	if( ++count >= size ) return true;
	for(uint8_t ix = 0; ix < size; ++ix){
		nfsix = (nfsix+1) % size;
		if(clients[nfsix] == INVALID_SOCKET) break;
	}
	return true;
}


bool ClientList::empty(){
	return count == 0;
}


bool ClientList::full(){
	return count >= size;
}


int ClientList::get(uint8_t ix){
	if( ix >= size ) return INVALID_SOCKET;
	return clients[ix];
}


uint8_t ClientList::getCount(){
	return count;
}


bool ClientList::remove(int socket){
	for(uint8_t ix = 0; ix < size; ++ix){
		if(clients[ix] != socket) continue;
		clients[ix] = INVALID_SOCKET;
		nfsix = ix;
		--count;
		return true;
	}
	return false;
}

bool ClientList::removeAt(uint8_t ix){
	if((ix >= size) || (clients[ix] < 0))
		return false;
	clients[ix] = INVALID_SOCKET;
	nfsix = ix;
	--count;
	return true;
}


int ClientList::operator[](uint8_t ix) const{
	if( ix >= size ) return INVALID_SOCKET;
	return clients[ix];
}