#include <es_system.h>

						WiiThread::WiiThread			(ThreadFunction aThreadFunction, void* aUserData) : 
	Function(aThreadFunction),
	UserData(aUserData),
	Thread(0),
	Result(0)
{
	LWP_CreateThread(&Thread, ThreadWrapper, (void*)this, 0, 128 * 1024, 64);
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
	WiiThread* thread = (WiiThread*)aUserData;
	return (void*)thread->Function(thread->UserData);
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
