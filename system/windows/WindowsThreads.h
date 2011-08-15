#pragma once

class								WindowsThread : public ESThread
{
	public:
									WindowsThread	(ThreadFunction aThreadFunction, void* aUserData); //External

		virtual						~WindowsThread		(); //External
		virtual int32_t				Wait				(); //External

	private:
		HANDLE						Thread;
		int32_t						Result;
		bool						Dead;
};

class								WindowsMutex : public ESMutex
{
	public:
									WindowsMutex		(); //External
		virtual						~WindowsMutex		(); //External

		void						Lock				(); //External
		void						Unlock				(); //External

	private:
		HANDLE						Mutex;
};

class								WindowsSemaphore : public ESSemaphore
{
	public:
									WindowsSemaphore	(uint32_t aValue); //External
		virtual						~WindowsSemaphore	(); //External

		virtual uint32_t			GetValue			(); //External
		virtual void				Post				(); //External
		virtual void				Wait				(); //External

	private:
		WindowsMutex				Sadness;

		HANDLE						Semaphore;
		LONG						Value;
};



class								WindowsThreads : public ESThreads
{
	public:
		virtual ESThread*			MakeThread			(ThreadFunction aThreadFunction, void* aUserData) {return new WindowsThread(aThreadFunction, aUserData);};
		virtual ESMutex*			MakeMutex			() {return new WindowsMutex();};
		virtual ESSemaphore*		MakeSemaphore		(uint32_t aValue) {return new WindowsSemaphore(aValue);};
};

