#pragma once

#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#define NO_READDIR
#define COMPLEX_VOLUMES

class				PlatformHelpers
{
	public:
		static uint32_t					GetTicks					()
		{
			return SDL_GetTicks();
		}

		static void						Sleep						(uint32_t aMilliseconds)
		{
			SDL_Delay(aMilliseconds);
		}

		template<typename T>
		static bool						ListDirectory				(const std::string& aPath, T& aOutput)
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

		template<typename T>
		static bool						ListVolumes					(T& aOutput)
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
};


