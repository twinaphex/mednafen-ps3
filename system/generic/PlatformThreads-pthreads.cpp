#include <es_system.h>

#include <pthread.h>
#include <semaphore.h>

struct						ESPlatformThreadPrivate
{
	ThreadFunction			Function;
	void*					UserData;

	pthread_t				Thread;
	pthread_attr_t			Attributes;
	int32_t					Result;
	bool					Dead;
};

struct						ESPlatformMutexPrivate
{
	pthread_mutex_t			Mutex;
	pthread_mutexattr_t		Attributes;
};

struct						ESPlatformSemaphorePrivate
{
	sem_t					Semaphore;
};

static void*				pthreadsRunThread				(void* aData)
{
	ESPlatformThreadPrivate* data = (ESPlatformThreadPrivate*)aData;
	data->Result = data->Function(data->UserData);
	return 0;
}	

							ESThread::ESThread				(ThreadFunction aThreadFunction, void* aUserData) : 
	Data(new ESPlatformThreadPrivate)
{
	Data->Function = aThreadFunction;
	Data->UserData = aUserData;
	Data->Result = 0;
	Data->Dead = false;

	pthread_attr_init(&Data->Attributes);
	pthread_create(&Data->Thread, &Data->Attributes, pthreadsRunThread, Data);
}

							ESThread::~ESThread				()
{
	Wait();

	pthread_attr_destroy(&Data->Attributes);
	delete Data;
}

int32_t						ESThread::Wait					()
{
	if(!Data->Dead)
	{
		pthread_join(Data->Thread, 0);
	}

	Data->Dead = true;
	return Data->Result;
}

							ESMutex::ESMutex				() : Data(new ESPlatformMutexPrivate)
{
	pthread_mutexattr_init(&Data->Attributes);
	pthread_mutex_init(&Data->Mutex, &Data->Attributes);
}

							ESMutex::~ESMutex				()
{
	pthread_mutex_destroy(&Data->Mutex);
	pthread_mutexattr_destroy(&Data->Attributes);

	delete Data;
}

void						ESMutex::Lock					()
{
	pthread_mutex_lock(&Data->Mutex);
}

void						ESMutex::Unlock					()
{
	pthread_mutex_unlock(&Data->Mutex);
}

							ESSemaphore::ESSemaphore		(uint32_t aValue) : Data(new ESPlatformSemaphorePrivate)
{
	sem_init(&Data->Semaphore, 0, aValue);
}

							ESSemaphore::~ESSemaphore		()
{
	sem_destroy(&Data->Semaphore);
	delete Data;
}

uint32_t					ESSemaphore::GetValue			()
{
	int32_t value;
	sem_getvalue(&Data->Semaphore, &value);
	return (value < 0) ? 0 : value;
}

void						ESSemaphore::Post				()
{
	sem_post(&Data->Semaphore);
}

void						ESSemaphore::Wait				()
{
	sem_wait(&Data->Semaphore);
}

void						ESThreads::Initialize			()
{

}

void						ESThreads::Shutdown				()
{

}



