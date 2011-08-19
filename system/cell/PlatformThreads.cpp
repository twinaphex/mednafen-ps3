#include <es_system.h>

struct					ESPlatformThreadPrivate
{
	sys_ppu_thread_t	Thread;
	int32_t				Result;
	bool				Dead;
};

struct					ESPlatformMutexPrivate
{
	sys_mutex_t			Mutex;
};

struct					ESPlatformSemaphorePrivate
{
	sys_semaphore_t		Semaphore;
};

struct					ThreadStart
{
	ThreadFunction		Function;
	void*				Data;
};

static void				ThreadWrapper					(uint64_t aUserData)
{
	ThreadStart* thread = (ThreadStart*)aUserData;
	int32_t result = thread->Function(thread->Data);
	sys_ppu_thread_exit(result);
}


						ESThread::ESThread				(ThreadFunction aThreadFunction, void* aUserData) : 
	Data(new ESPlatformThreadPrivate())
{
	ThreadStart startData = {aThreadFunction, aUserData};
	sys_ppu_thread_create(&Data->Thread, ThreadWrapper, (uint64_t)&startData, 500, 65536, SYS_PPU_THREAD_CREATE_JOINABLE, "\0");

	Data->Thread = 0;
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
		Data->Dead = true;

		uint64_t result64;
		sys_ppu_thread_join(Data->Thread, &result64);
		Data->Result = (int32_t)result64;
	}

	return Data->Result;
}

						ESMutex::ESMutex				() : Data(new ESPlatformMutexPrivate())
{
	sys_mutex_attribute_t attr;
	sys_mutex_attribute_initialize(attr);
	sys_mutex_create(&Data->Mutex, &attr);
}

						ESMutex::~ESMutex				()
{
	sys_mutex_destroy(Data->Mutex);

	delete Data;
}

void					ESMutex::Lock					()
{
	sys_mutex_lock(Data->Mutex, 0);
}

void					ESMutex::Unlock					()
{
	sys_mutex_unlock(Data->Mutex);
}

						ESSemaphore::ESSemaphore		(uint32_t aValue) : Data(new ESPlatformSemaphorePrivate())
{
	sys_semaphore_attribute_t attr;
	sys_semaphore_attribute_initialize(attr);
	sys_semaphore_create(&Data->Semaphore, &attr, aValue, aValue);
}

						ESSemaphore::~ESSemaphore		()
{
	sys_semaphore_destroy(Data->Semaphore);

	delete Data;
}

uint32_t				ESSemaphore::GetValue			()
{
	sys_semaphore_value_t val;
	sys_semaphore_get_value(Data->Semaphore, &val);
	return val;
}

void					ESSemaphore::Post				()
{
	sys_semaphore_post(Data->Semaphore, 1);
}

void					ESSemaphore::Wait				()
{
	sys_semaphore_wait(Data->Semaphore, 0);
}

void					ESThreads::Initialize			()
{

}

void					ESThreads::Shutdown				()
{

}

