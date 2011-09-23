#include <es_system.h>
#include "src/ESException.h"
#include <winsock2.h>

namespace
{
	WSADATA					WinsockData;
}

struct						ESPlatformSocketPrivate
{
	unsigned int			Socket;
};

							ESSocket::ESSocket				(const char* aHost, uint32_t aPort) : Data(new ESPlatformSocketPrivate)
{
	Data->Socket = socket(AF_INET, SOCK_STREAM, 0);
	ESException::ErrorCheck(Data->Socket != INVALID_SOCKET, "WindowsNetwork: Could not open socket");

	//TODO: gethostby name is appently evil?
	struct hostent* server = gethostbyname(aHost);
	if(server == 0)
	{
		closesocket(Data->Socket);
		ESException::ErrorCheck(0, "WindowsNetwork: Host look up failed");
	}

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_port = htons(aPort);	
	serv_addr.sin_family = AF_INET;
	memcpy((char *)&serv_addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);

	if(-1 != connect(Data->Socket, (sockaddr*)&serv_addr, sizeof(serv_addr)))
	{
		closesocket(Data->Socket);
		ESException::ErrorCheck(0, "WindowsNetwork: connect() failed");
	}
}

							ESSocket::~ESSocket				()
{
	closesocket(Data->Socket);

	delete Data;
}

uint32_t					ESSocket::ReadString			(void* aBuffer, uint32_t aLength)
{
	char* buff = (char*)aBuffer;

	for(int i = 0; i != aLength; i ++)
	{
		int count = recv(Data->Socket, &buff[i], 1, 0);

		if(0 == count || buff[i] == 0x0A)
		{
			return i;
		}

		ESException::ErrorCheck(count >= 0, "WindowsNetwork: failed to read socket");
	}

	return aLength;
}

uint32_t					ESSocket::Read					(void* aBuffer, uint32_t aLength)
{
	char* buff = (char*)aBuffer;

	int count = recv(Data->Socket, buff, aLength, 0);

	ESException::ErrorCheck(count >= 0, "WindowsNetwork: failed to read socket");

	return count;
}

void						ESSocket::Write					(const void* aBuffer, uint32_t aLength)
{
	ESException::ErrorCheck(aLength == send(Data->Socket, (const char*)aBuffer, aLength, 0), "WindowsNetwork: failed to write socket");
}

void						ESNetwork::Initialize			()
{
	WSAStartup(MAKEWORD(2,2), &WinsockData);
}

void						ESNetwork::Shutdown				()
{
	WSACleanup();
}

ESSocket*					ESNetwork::OpenSocket			(const char* aHost, uint32_t aPort)
{
	return new ESSocket(aHost, aPort);
}


