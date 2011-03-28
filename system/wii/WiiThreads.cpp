#include <es_system.h>

						WiiThread::WiiThread			(ThreadFunction aThreadFunction, void* aUserData) : 
	Function(aThreadFunction),
	Thread(0),
	Result(0)
{
	void* threaddata[2] = {(void*)Function, aUserData};
	LWP_CreateThread(&Thread, ThreadWrapper, threaddata, 0, 0, 64);
}

						WiiThread::~WiiThread			()
{
	Wait();
}

int32_t					WiiThread::Wait					()
{
	void* result;
	LWP_JoinThread(Thread, &result);

	return (int32_t)Result;
}

void*					WiiThread::ThreadWrapper		(void* aUserData)
{
	void** userdata = (void**)aUserData;
	return (void*)((ThreadFunction)userdata[0])(userdata[1]);
}

						WiiMutex::WiiMutex				()
{
	LWP_MutexInit(&Mutex, false);
}

						WiiMutex::~WiiMutex				()
{
	LWP_MutexDestroy(Mutex);
}

void					WiiMutex::Lock					()
{
	LWP_MutexLock(Mutex);
}

void					WiiMutex::Unlock				()
{
	LWP_MutexUnlock(Mutex);
}
