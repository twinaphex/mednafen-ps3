#include <ps3_system.h>

							WiiSocket::WiiSocket			(const char* aHost, uint32_t aPort)
{
}

							WiiSocket::~WiiSocket			()
{
}

uint32_t					WiiSocket::ReadString			(void* aBuffer, uint32_t aLength)
{
	return 0;
}

uint32_t					WiiSocket::Read					(void* aBuffer, uint32_t aLength)
{
	return 0;
}

void						WiiSocket::Write				(const void* aBuffer, uint32_t aLength)
{
}

							WiiNetwork::WiiNetwork			()
{
}

							WiiNetwork::~WiiNetwork			()
{
}

ESSocket*					WiiNetwork::OpenSocket			(const char* aHost, uint32_t aPort)
{
	return new WiiSocket(aHost, aPort);
}

