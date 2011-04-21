#pragma once

class								DorkSocket : public ESSocket
{
	public:
									DorkSocket			(const char* aHost, uint32_t aPort);
		virtual						~DorkSocket			();

		virtual uint32_t			ReadString			(void* aBuffer, uint32_t aLength);
		virtual uint32_t			Read				(void* aBuffer, uint32_t aLength);
		virtual void				Write				(const void* aBuffer, uint32_t aLength);

	protected:
		int							Socket;
};

class								DorkNetwork : public ESNetwork
{
	public:
									DorkNetwork			();
									~DorkNetwork		();

		virtual ESSocket*			OpenSocket			(const char* aHost, uint32_t aPort);
};

