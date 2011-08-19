#include <es_system.h>

struct					ESPlatformThreadPrivate
{
	SDL_Thread*			Thread;
	int32_t				Result;
	bool				Dead;
};

struct					ESPlatformMutexPrivate
{
	SDL_mutex*			Mutex;
};

struct					ESPlatformSemaphorePrivate
{
	SDL_sem*			Semaphore;
};


						ESThread::ESThread				(ThreadFunction aThreadFunction, void* aUserData) : 
	Data(new ESPlatformThreadPrivate)
{
	Data->Thread = SDL_CreateThread(aThreadFunction, aUserData);
	Data->Result = 0;
	Data->Dead = false;
}

						ESThread::~ESThread				()
{
	Wait();

	delete Data;
}

int32_t					ESThread::Wait					()
{
	if(!Data->Dead)
	{
		SDL_WaitThread(Data->Thread, &Data->Result);
	}

	Data->Dead = true;
	return Data->Result;
}

						ESMutex::ESMutex				() : Data(new ESPlatformMutexPrivate)
{
	Data->Mutex = SDL_CreateMutex();
}

						ESMutex::~ESMutex				()
{
	SDL_DestroyMutex(Data->Mutex);

	delete Data;
}

void					ESMutex::Lock					()
{
	SDL_mutexP(Data->Mutex);
}

void					ESMutex::Unlock					()
{
	SDL_mutexV(Data->Mutex);
}

						ESSemaphore::ESSemaphore		(uint32_t aValue) : Data(new ESPlatformSemaphorePrivate)
{
	Data->Semaphore = SDL_CreateSemaphore(aValue);
}

						ESSemaphore::~ESSemaphore		()
{
	SDL_DestroySemaphore(Data->Semaphore);

	delete Data;
}

uint32_t				ESSemaphore::GetValue			()
{
	return SDL_SemValue(Data->Semaphore);
}

void					ESSemaphore::Post				()
{
	SDL_SemPost(Data->Semaphore);
}

void					ESSemaphore::Wait				()
{
	SDL_SemWait(Data->Semaphore);
}

void					ESThreads::Initialize			()
{

}

void					ESThreads::Shutdown				()
{

}



