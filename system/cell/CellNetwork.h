#pragma once

class								CellSocket : public ESSocket
{
	public:
									CellSocket			(const char* aHost, uint32_t aPort);
		virtual						~CellSocket			();

		virtual uint32_t			ReadString			(void* aBuffer, uint32_t aLength);
		virtual uint32_t			Read				(void* aBuffer, uint32_t aLength);
		virtual void				Write				(const void* aBuffer, uint32_t aLength);

	protected:
		int							Socket;
};

class								CellNetwork : public ESNetwork
{
	public:
									CellNetwork			();
									~CellNetwork		();

		virtual ESSocket*			OpenSocket			(const char* aHost, uint32_t aPort);
};

