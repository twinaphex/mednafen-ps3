#include <SDL/SDL.h>

#ifndef __WIN32__
#include <sys/mman.h>
#else
#include <windows.h>
#endif

#include "Platform.h"

uint32_t					PlatformHelpers::GetTicks					()
{
	return SDL_GetTicks();
}

void						PlatformHelpers::Sleep						(uint32_t aMilliseconds)
{
	SDL_Delay(aMilliseconds);
}

void*						PlatformHelpers::AllocateExecutable			(uint32_t aSize)
{
#ifdef __WIN32__
	return VirtualAlloc(0, aSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#else
	return mmap(0, aSize, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
}

void						PlatformHelpers::FreeExecutable				(void* aData, uint32_t aSize)
{
#ifdef __WIN32__
	VirtualFree(aData, 0, MEM_FREE);
#else
	munmap(aData, aSize);
#endif
}

