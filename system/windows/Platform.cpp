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

