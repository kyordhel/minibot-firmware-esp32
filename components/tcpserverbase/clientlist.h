#pragma once

#include <cstdint>

namespace minibot::net{

/**
 * Stores a list of socket clients
 */
class ClientList{
public:
	ClientList(uint8_t size);
	~ClientList();

private:
	ClientList(const ClientList&)            = delete;
	ClientList& operator=(const ClientList&) = delete;

public:
	bool add(int socket);
	bool empty();
	bool full();
	int get(uint8_t ix);
	uint8_t getCount();
	bool remove(int socket);
	bool removeAt(uint8_t ix);

public:
	int operator[](uint8_t ix) const;

private:
	uint8_t size;
	uint8_t count;
	uint8_t nfsix; // Next free slot index
	int *clients;
};

} // End namespace
