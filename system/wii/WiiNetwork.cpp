#include <es_system.h>

							WiiSocket::WiiSocket			(const char* aHost, uint32_t aPort)
{
	Socket = net_socket(AF_INET, SOCK_STREAM, 0);
	if(Socket == -1)
	{
		throw ESException("L1ghtNetwork: Could not open socket");
	}

	//TODO: gethostby name is appently evil?
	struct hostent* server = net_gethostbyname(aHost);
	if(server == 0)
	{
		close(Socket);
		throw ESException("L1ghtNetwork: Host look up failed");
	}

	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_port = htons(aPort);	
	serv_addr.sin_family = AF_INET;
	memcpy((char *)&serv_addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);

	if(-1 == net_connect(Socket, (sockaddr*)&serv_addr, sizeof(serv_addr)))
	{
		close(Socket);
		throw ESException("L1ghtNetwork: connect() failed");
	}
}

							WiiSocket::~WiiSocket			()
{
	close(Socket);
}

uint32_t					WiiSocket::ReadString			(void* aBuffer, uint32_t aLength)
{
	uint8_t* buff = (uint8_t*)aBuffer;

	for(int i = 0; i != aLength; i ++)
	{
		int count = net_recv(Socket, &buff[i], 1, 0);

		if(0 == count || buff[i] == 0x0A)
		{
			return i;
		}

		if(count < 0)
		{
			throw ESException("L1ghtNetwork: failed to read socket");
		}
	}

	return aLength;
}

uint32_t					WiiSocket::Read					(void* aBuffer, uint32_t aLength)
{
	uint8_t* buff = (uint8_t*)aBuffer;

	int count = net_recv(Socket, aBuffer, aLength, 0);

	if(count < 0)
	{
		throw ESException("L1ghtNetwork: failed to read socket");
	}

	return count;
}

void						WiiSocket::Write				(const void* aBuffer, uint32_t aLength)
{
	if(aLength != net_send(Socket, aBuffer, aLength, 0))
	{
//		throw ESException("L1ghtNetwork: failed to write socket");
	}
}

							WiiNetwork::WiiNetwork			()
{
	if_config(0, 0, 0, TRUE);
}

							WiiNetwork::~WiiNetwork			()
{
}

ESSocket*					WiiNetwork::OpenSocket			(const char* aHost, uint32_t aPort)
{
	return new WiiSocket(aHost, aPort);
}

