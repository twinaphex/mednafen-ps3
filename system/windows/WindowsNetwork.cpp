#include <es_system.h>
#include <winsock2.h>

namespace
{
	WSADATA					WinsockData;
}

							ESSocket::ESSocket				(const char* aHost, uint32_t aPort)
{
	Socket = socket(AF_INET, SOCK_STREAM, 0);
	ErrorCheck(Socket != INVALID_SOCKET, "WindowsNetwork: Could not open socket");

	//TODO: gethostby name is appently evil?
	struct hostent* server = gethostbyname(aHost);
	if(server == 0)
	{
		closesocket(Socket);
		ErrorCheck(0, "WindowsNetwork: Host look up failed");
	}

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_port = htons(aPort);	
	serv_addr.sin_family = AF_INET;
	memcpy((char *)&serv_addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);

	if(-1 != connect(Socket, (sockaddr*)&serv_addr, sizeof(serv_addr)))
	{
		closesocket(Socket);
		ErrorCheck(0, "WindowsNetwork: connect() failed");
	}
}

							ESSocket::~ESSocket				()
{
	closesocket(Socket);
}

uint32_t					ESSocket::ReadString			(void* aBuffer, uint32_t aLength)
{
	char* buff = (char*)aBuffer;

	for(int i = 0; i != aLength; i ++)
	{
		int count = recv(Socket, &buff[i], 1, 0);

		if(0 == count || buff[i] == 0x0A)
		{
			return i;
		}

		ErrorCheck(count >= 0, "WindowsNetwork: failed to read socket");
	}

	return aLength;
}

uint32_t					ESSocket::Read					(void* aBuffer, uint32_t aLength)
{
	char* buff = (char*)aBuffer;

	int count = recv(Socket, aBuffer, aLength, 0);

	ErrorCheck(count >= 0, "WindowsNetwork: failed to read socket");

	return count;
}

void						ESSocket::Write					(const void* aBuffer, uint32_t aLength)
{
	ErrorCheck(aLength == send(Socket, (const char*)aBuffer, aLength, 0), "WindowsNetwork: failed to write socket");
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


