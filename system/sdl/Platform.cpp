#include <SDL/SDL.h>

#ifndef __WIN32__
# include <sys/mman.h>
#else
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <shlobj.h>
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

#ifdef __WIN32__
bool						PlatformHelpers::ListDirectory				(const std::string& aPath, std::list<std::string>& aOutput)
{
	WIN32_FIND_DATA fileData;
	HANDLE findHandle = FindFirstFile((aPath + "\\*").c_str(), &fileData);

	if(findHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(strcmp(fileData.cFileName, ".") && strcmp(fileData.cFileName, ".."))
			{
				aOutput.push_back(std::string(fileData.cFileName) + ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? "/" : ""));
			}
		}	while(FindNextFile(findHandle, &fileData));

		FindClose(findHandle);
		return true;
	}

	return false;
}

bool						PlatformHelpers::ListVolumes				(std::list<std::string>& aOutput)
{
	DWORD drives = GetLogicalDrives();

	for(int i = 0; i != 26; i ++)
	{
		if(drives & (1 << i))
		{
			aOutput.push_back(std::string(1, 'A' + i) + ":\\");
		}
	}

	return true;
}
#endif
