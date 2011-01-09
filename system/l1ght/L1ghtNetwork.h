#ifndef L1GTHNETWORK_H
#define L1GTHNETWORK_H

class								L1ghtSocket : public ESSocket
{
	public:
									L1ghtSocket			(const char* aHost, uint32_t aPort);
		virtual						~L1ghtSocket		();

		virtual uint32_t			ReadString			(void* aBuffer, uint32_t aLength);
		virtual uint32_t			Read				(void* aBuffer, uint32_t aLength);
		virtual void				Write				(const void* aBuffer, uint32_t aLength);

	protected:
		int							Socket;
};

class								L1ghtNetwork : public ESNetwork
{
	public:
									L1ghtNetwork		();
									~L1ghtNetwork		();

		virtual ESSocket*			OpenSocket			(const char* aHost, uint32_t aPort);
};

#endif



