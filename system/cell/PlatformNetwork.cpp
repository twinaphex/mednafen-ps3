#include <es_system.h>
#include "src/ESException.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>


struct						ESPlatformSocketPrivate
{
	int						Socket;
};


							ESSocket::ESSocket				(const char* aHost, uint32_t aPort) : Data(new ESPlatformSocketPrivate)
{
	Data->Socket = socket(AF_INET, SOCK_STREAM, 0);
	ESException::ErrorCheck(Data->Socket != -1, "CellNetwork: Could not open socket");

	//TODO: gethostby name is appently evil?
	struct hostent* server = gethostbyname(aHost);
	if(server == 0)
	{
		close(Data->Socket);
		ESException::ErrorCheck(0, "CellNetwork: Host look up failed");
	}

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_port = htons(aPort);	
	serv_addr.sin_family = AF_INET;
	memcpy((char *)&serv_addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);

	if(-1 != connect(Data->Socket, (sockaddr*)&serv_addr, sizeof(serv_addr)))
	{
		close(Data->Socket);
		ESException::ErrorCheck(0, "CellNetwork: connect() failed");
	}
}

							ESSocket::~ESSocket				()
{
	close(Data->Socket);

	delete Data;
}

uint32_t					ESSocket::ReadString			(void* aBuffer, uint32_t aLength)
{
	uint8_t* buff = (uint8_t*)aBuffer;

	for(int i = 0; i != aLength; i ++)
	{
		int count = recv(Data->Socket, &buff[i], 1, 0);

		if(0 == count || buff[i] == 0x0A)
		{
			return i;
		}

		ESException::ErrorCheck(count >= 0, "CellNetwork: failed to read socket");
	}

	return aLength;
}

uint32_t					ESSocket::Read					(void* aBuffer, uint32_t aLength)
{
	uint8_t* buff = (uint8_t*)aBuffer;

	int count = recv(Data->Socket, aBuffer, aLength, 0);

	ESException::ErrorCheck(count >= 0, "CellNetwork: failed to read socket");

	return count;
}

void						ESSocket::Write					(const void* aBuffer, uint32_t aLength)
{
	ESException::ErrorCheck(aLength == send(Data->Socket, aBuffer, aLength, 0), "CellNetwork: failed to write socket");
}

void						ESNetwork::Initialize			()
{
//TODO:
//	netInitialize();
}

void						ESNetwork::Shutdown				()
{
}

ESSocket*					ESNetwork::OpenSocket			(const char* aHost, uint32_t aPort)
{
	return new ESSocket(aHost, aPort);
}


