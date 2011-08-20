#include <es_system.h>
#include <windows.h>

struct					ESPlatformThreadPrivate
{
	ThreadFunction		Function;
	void*				UserData;

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


static DWORD WINAPI		WindowsRunThread						(void* aData)
{
	ESPlatformThreadPrivate* data = (ESPlatformThreadPrivate*)aData;
	return data->Function(data->UserData);
}	


						ESThread::ESThread						(ThreadFunction aThreadFunction, void* aUserData) : Data(new ESPlatformThreadPrivate())
{
	Data->Function = aThreadFunction;
	Data->UserData = aUserData;
	Data->Result = 0;
	Data->Dead = false;
	Data->Thread = CreateThread(0, 0, WindowsRunThread, Data, 0, 0);
}

						ESThread::~ESThread						()
{
	if(!Data->Dead)
	{
		WaitForSingleObject(Data->Thread, INFINITE);
	}

	delete Data;
}

int32_t					ESThread::Wait							()
{
	if(!Data->Dead)
	{
		Data->Result = WaitForSingleObject(Data->Thread, INFINITE);
	}

	Data->Dead = true;
	return Data->Result;
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
	Data->Sadness.Lock();
	uint32_t result = Data->Value;
	Data->Sadness.Unlock();
	return result;
}

void					ESSemaphore::Post						()
{
	Data->Sadness.Lock();
	ReleaseSemaphore(Data->Semaphore, 1, &Data->Value);
	Data->Sadness.Unlock();
}

void					ESSemaphore::Wait						()
{
	Data->Sadness.Lock();
	Data->Value -= 1;
	Data->Sadness.Unlock();
	WaitForSingleObject(Data->Semaphore, INFINITE);
}

void					ESThreads::Initialize			()
{

}

void					ESThreads::Shutdown				()
{

}



