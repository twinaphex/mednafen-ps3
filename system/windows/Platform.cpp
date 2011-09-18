#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <mmsystem.h>
#include "Platform.h"

uint32_t					PlatformHelpers::GetTicks					()
{
	return timeGetTime();
}

void						PlatformHelpers::Sleep						(uint32_t aMilliseconds)
{
	::Sleep(aMilliseconds);
}

void*						PlatformHelpers::AllocateExecutable			(uint32_t aSize)
{
	return VirtualAlloc(0, aSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

void						PlatformHelpers::FreeExecutable				(void* aData, uint32_t aSize)
{
	VirtualFree(aData, 0, MEM_FREE);
}

