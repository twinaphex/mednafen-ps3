#ifndef SDLNETWORK_H
#define SDLNETWORK_H

class								SDLSocket : public ESSocket
{
	public:
									SDLSocket			(const char* aHost, uint32_t aPort);
		virtual						~SDLSocket			();

		virtual uint32_t			ReadString			(void* aBuffer, uint32_t aLength);
		virtual uint32_t			Read				(void* aBuffer, uint32_t aLength);
		virtual void				Write				(const void* aBuffer, uint32_t aLength);

	protected:
		TCPsocket					Socket;
		IPaddress					Connection;
};

class								SDLNetwork : public ESNetwork
{
	public:
									SDLNetwork			();
									~SDLNetwork			();

		virtual ESSocket*			OpenSocket			(const char* aHost, uint32_t aPort);
};

#endif



