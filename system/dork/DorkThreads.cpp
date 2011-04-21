#include <es_system.h>

						DorkThread::DorkThread			(ThreadFunction aThreadFunction, void* aUserData) : 
	Function(aThreadFunction),
	UserData(aUserData),
	Thread(0),
	Result(0),
	Dead(false)
{
	sys_ppu_thread_create(&Thread, ThreadWrapper, (uint64_t)this, 500, 65536, SYS_PPU_THREAD_CREATE_JOINABLE, "\0");
}

						DorkThread::~DorkThread			()
{
	Wait();
}

int32_t					DorkThread::Wait				()
{
	if(!Dead)
	{
		Dead = true;

		uint64_t result64;
		sys_ppu_thread_join(Thread, &result64);
		Result = (int32_t)result64;
	}

	return Result;
}

void					DorkThread::ThreadWrapper		(uint64_t aUserData)
{
	DorkThread* thread = (DorkThread*)aUserData;
	int32_t result = thread->Function(thread->UserData);
	sys_ppu_thread_exit(result);
}

						DorkMutex::DorkMutex			() :
	Mutex(0)
{
	sys_mutex_attribute_t attr;
	sys_mutex_attribute_initialize(attr);
	sys_mutex_create(&Mutex, &attr);
}

						DorkMutex::~DorkMutex			()
{
	sys_mutex_destroy(Mutex);
}

void					DorkMutex::Lock					()
{
	sys_mutex_lock(Mutex, 0);
}

void					DorkMutex::Unlock				()
{
	sys_mutex_unlock(Mutex);
}
