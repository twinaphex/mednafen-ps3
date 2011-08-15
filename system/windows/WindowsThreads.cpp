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

						WindowsThread::WindowsThread			(ThreadFunction aThreadFunction, void* aUserData) : 
	Thread(0),
	Result(0),
	Dead(false)
{
	ThreadStart data = {aThreadFunction, aUserData};
	Thread = CreateThread(0, 0, WindowsRunThread, &data, 0, 0);
}

						WindowsThread::~WindowsThread			()
{
	if(!Dead)
	{
		WaitForSingleObject(Thread, INFINITE);
	}
}

int32_t					WindowsThread::Wait						()
{
	if(!Dead)
	{
		Result = WaitForSingleObject(Thread, INFINITE);
	}

	Dead = true;
	return Result;
}

						WindowsMutex::WindowsMutex				()
{
	Mutex = CreateMutex(0, false, 0);
}

						WindowsMutex::~WindowsMutex				()
{
	CloseHandle(Mutex);
}

void					WindowsMutex::Lock						()
{
	WaitForSingleObject(Mutex, INFINITE);
}

void					WindowsMutex::Unlock					()
{
	ReleaseMutex(Mutex);
}

						WindowsSemaphore::WindowsSemaphore		(uint32_t aValue) : Value(aValue)
{
	Semaphore = CreateSemaphore(0, aValue, 1000, 0);
}

						WindowsSemaphore::~WindowsSemaphore		()
{
	CloseHandle(Semaphore);
}

uint32_t				WindowsSemaphore::GetValue				()
{
	return Value;
}

void					WindowsSemaphore::Post					()
{
	Sadness.Lock();
	ReleaseSemaphore(Semaphore, 1, &Value);
	Sadness.Unlock();
}

void					WindowsSemaphore::Wait					()
{
	WaitForSingleObject(Semaphore, INFINITE);
	Sadness.Lock();
	Value -= 1;
	Sadness.Unlock();
}

