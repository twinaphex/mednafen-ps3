#include <es_system.h>
#include <windows.h>

namespace
{
	struct				ThreadStart
	{
		ThreadFunction	Function;
		void*			UserData;
	};

	DWORD WINAPI		WindowsRunThread						(void* aData)
	{
		ThreadStart* data = (ThreadStart*)aData;
		return data->Function(data->UserData);
	}	
};

struct					ESPlatformThreadPrivate
{
	HANDLE				Thread;
	int32_t				Result;
	bool				Dead;
};

struct					ESPlatformMutexPrivate
{
	HANDLE				Mutex;
};

struct					ESPlatformSemaphorePrivate
{
	ESMutex				Sadness;

	HANDLE				Semaphore;
	LONG				Value;
};


						ESThread::ESThread						(ThreadFunction aThreadFunction, void* aUserData) : Data(new ESPlatformThreadPrivate())
{
	ThreadStart data = {aThreadFunction, aUserData};
	Thread = CreateThread(0, 0, WindowsRunThread, &data, 0, 0);
}

						ESThread::~ESThread						()
{
	if(!Dead)
	{
		WaitForSingleObject(Thread, INFINITE);
	}

	delete Data;
}

int32_t					ESThread::Wait							()
{
	if(!Dead)
	{
		Result = WaitForSingleObject(Thread, INFINITE);
	}

	Dead = true;
	return Result;
}

						ESMutex::ESMutex						() : Data(new ESPlatformMutexPrivate())
{
	Data->Mutex = CreateMutex(0, false, 0);
}

						ESMutex::~ESMutex						()
{
	CloseHandle(Data->Mutex);
}

void					ESMutex::Lock							()
{
	WaitForSingleObject(Data->Mutex, INFINITE);
}

void					ESMutex::Unlock							()
{
	ReleaseMutex(Data->Mutex);
}

						ESSemaphore::ESSemaphore				(uint32_t aValue) : Data(new ESPlatformSemaphorePrivate())
{
	Data->Semaphore = CreateSemaphore(0, aValue, 1000, 0);
	Data->Value = aValue;
}

						ESSemaphore::~ESSemaphore				()
{
	CloseHandle(Data->Semaphore);

	delete Data;
}

uint32_t				ESSemaphore::GetValue					()
{
	return Value;
}

void					ESSemaphore::Post						()
{
	Data->Sadness.Lock();
	ReleaseSemaphore(Data->Semaphore, 1, &Data->Value);
	Data->Sadness.Unlock();
}

void					ESSemaphore::Wait						()
{
	WaitForSingleObject(Data->Semaphore, INFINITE);
	Data->Sadness.Lock();
	Data->Value -= 1;
	Data->Sadness.Unlock();
}

void					ESThreads::Initialize			()
{

}

void					ESThreads::Shutdown				()
{

}



