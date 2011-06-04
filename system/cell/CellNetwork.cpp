#include <es_system.h>

							CellSocket::CellSocket			(const char* aHost, uint32_t aPort)
{
	Socket = socket(AF_INET, SOCK_STREAM, 0);
	if(Socket == -1)
	{
		throw ESException("CellNetwork: Could not open socket");
	}

	//TODO: gethostby name is appently evil?
	struct hostent* server = gethostbyname(aHost);
	if(server == 0)
	{
		close(Socket);
		throw ESException("CellNetwork: Host look up failed");
	}

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_port = htons(aPort);	
	serv_addr.sin_family = AF_INET;
	memcpy((char *)&serv_addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);

	if(-1 == connect(Socket, (sockaddr*)&serv_addr, sizeof(serv_addr)))
	{
		close(Socket);
		throw ESException("CellNetwork: connect() failed");
	}
}

							CellSocket::~CellSocket			()
{
	close(Socket);
}

uint32_t					CellSocket::ReadString			(void* aBuffer, uint32_t aLength)
{
	uint8_t* buff = (uint8_t*)aBuffer;

	for(int i = 0; i != aLength; i ++)
	{
		int count = recv(Socket, &buff[i], 1, 0);

		if(0 == count || buff[i] == 0x0A)
		{
			return i;
		}

		if(count < 0)
		{
			throw ESException("CellNetwork: failed to read socket");
		}
	}

	return aLength;
}

uint32_t					CellSocket::Read				(void* aBuffer, uint32_t aLength)
{
	uint8_t* buff = (uint8_t*)aBuffer;

	int count = recv(Socket, aBuffer, aLength, 0);

	if(count < 0)
	{
		throw ESException("CellNetwork: failed to read socket");
	}

	return count;
}

void						CellSocket::Write				(const void* aBuffer, uint32_t aLength)
{
	if(aLength != send(Socket, aBuffer, aLength, 0))
	{
		throw ESException("CellNetwork: failed to write socket");
	}
}

							CellNetwork::CellNetwork		()
{
//TODO:
//	netInitialize();
}

							CellNetwork::~CellNetwork		()
{
}

ESSocket*					CellNetwork::OpenSocket		(const char* aHost, uint32_t aPort)
{
	return new CellSocket(aHost, aPort);
}


