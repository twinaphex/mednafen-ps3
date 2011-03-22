#ifndef WIINETWORK_H
#define WIINETWORK_H

class								WiiSocket : public ESSocket
{
	public:
									WiiSocket			(const char* aHost, uint32_t aPort);
		virtual						~WiiSocket			();

		virtual uint32_t			ReadString			(void* aBuffer, uint32_t aLength);
		virtual uint32_t			Read				(void* aBuffer, uint32_t aLength);
		virtual void				Write				(const void* aBuffer, uint32_t aLength);

	protected:
};

class								WiiNetwork : public ESNetwork
{
	public:
									WiiNetwork			();
									~WiiNetwork			();

		virtual ESSocket*			OpenSocket			(const char* aHost, uint32_t aPort);
};

#endif



