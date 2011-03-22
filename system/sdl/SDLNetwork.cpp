#include <es_system.h>

							SDLSocket::SDLSocket			(const char* aHost, uint32_t aPort)
{
	if(-1 == SDLNet_ResolveHost(&Connection, aHost, aPort))
	{
		throw ESException("SDL_net failed to resolve host: %s", SDLNet_GetError());
	}

	Socket = SDLNet_TCP_Open(&Connection);
	if(Socket == 0)
	{
		throw ESException("SDL_net failed to open connection: %s", SDLNet_GetError());
	}
}

							SDLSocket::~SDLSocket			()
{
	SDLNet_TCP_Close(Socket);
}

uint32_t					SDLSocket::ReadString			(void* aBuffer, uint32_t aLength)
{
	uint8_t* buff = (uint8_t*)aBuffer;

	for(int i = 0; i != aLength; i ++)
	{
		int count = SDLNet_TCP_Recv(Socket, &buff[i], 1);

		if(0 == count || buff[i] == 0x0A)
		{
			return i;
		}

		if(count < 0)
		{
			throw ESException("SDL_net failed to read socket: %s", SDLNet_GetError());
		}
	}

	return aLength;
}

uint32_t					SDLSocket::Read					(void* aBuffer, uint32_t aLength)
{
	uint8_t* buff = (uint8_t*)aBuffer;

	int count = SDLNet_TCP_Recv(Socket, aBuffer, aLength);

	if(count < 0)
	{
		throw ESException("SDL_net failed to read socket: %s", SDLNet_GetError());
	}

	return count;
}

void						SDLSocket::Write				(const void* aBuffer, uint32_t aLength)
{
	if(aLength != SDLNet_TCP_Send(Socket, aBuffer, aLength))
	{
		throw ESException("SDL_net failed to write socket: %s", SDLNet_GetError());
	}
}

							SDLNetwork::SDLNetwork			()
{
	if(-1 == SDLNet_Init())
	{
		throw ESException("SDL_net failed to initialize: %s", SDLNet_GetError());
	}
}

							SDLNetwork::~SDLNetwork			()
{
	SDLNet_Quit();
}

ESSocket*					SDLNetwork::OpenSocket			(const char* aHost, uint32_t aPort)
{
	return new SDLSocket(aHost, aPort);
}

