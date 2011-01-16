#ifndef ESNETWORK_H
#define ESNETWORK_H

class								ESSocket
{
	public:
		virtual						~ESSocket			() {};

		virtual uint32_t			ReadString			(void* aBuffer, uint32_t aLength) = 0;
		virtual uint32_t			Read				(void* aBuffer, uint32_t aLength) = 0;
		virtual void				Write				(const void* aBuffer, uint32_t aLength) = 0;
};

class								ESNetwork
{
	public:
		virtual ESSocket*			OpenSocket			(const char* aHost, uint32_t aPort) = 0;
};

#endif