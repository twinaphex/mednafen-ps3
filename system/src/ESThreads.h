#pragma once

typedef int							(*ThreadFunction)	(void*);
class								ESPlatformThreadPrivate;
class								ESThread
{
	public:
									ESThread			(ThreadFunction aFunction, void* aUserData);
									~ESThread			();

		int32_t						Wait				();

	private:
		ESPlatformThreadPrivate*	Data;
};

class								ESPlatformMutexPrivate;
class								ESMutex
{
	public:
									ESMutex				();
									~ESMutex			();

		void						Lock				();
		void						Unlock				();

	private:
		ESPlatformMutexPrivate*		Data;

};

class								ESPlatformSemaphorePrivate;
class								ESSemaphore
{
	public:
									ESSemaphore			(uint32_t aValue);			
									~ESSemaphore		();

		uint32_t					GetValue			();
		void						Post				();
		void						Wait				();

	private:
		ESPlatformSemaphorePrivate*	Data;

};

class								ESThreads
{
	public:
		static void					Initialize			();
		static void					Shutdown			();

		static ESThread*			MakeThread			(ThreadFunction aFunction, void* aUserData) {return new ESThread(aFunction, aUserData);}
		static ESMutex*				MakeMutex			() {return new ESMutex();}
		static ESSemaphore*			MakeSemaphore		(uint32_t aValue) {return new ESSemaphore(aValue);}
};

