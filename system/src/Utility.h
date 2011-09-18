#pragma once

#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdint.h>
#include <vector>
#include <string>
#include "Platform.h"

class				Utility
{
	public:
		static uint32_t					GetTicks					() {return PlatformHelpers::GetTicks();}
		static void						Sleep						(uint32_t aMilliseconds) {return PlatformHelpers::Sleep(aMilliseconds);}

		static int32_t					Clamp						(int32_t aValue, int32_t aMin, int32_t aMax) {return std::min(std::max(aValue, aMin), aMax);}

		static void						StringToVector				(std::vector<std::string>& aOutput, const std::string& aString, char aSeparate);
		static std::string&				VectorToString				(std::string& aOutput, const std::vector<std::string>& aStrings, char aSeparate);

		static void*					AllocateExecutable			(uint32_t aSize);
		static void						FreeExecutable				(void* aData, uint32_t aSize);

		static char*					VAPrint						(char* aBuffer, uint32_t aLength, const char* aFormat, ...);
		static char*					VAPrintD					(const char* aFormat, ...);
		static void						CenterAndScale				(uint32_t& aX, uint32_t& aY, uint32_t& aWidth, uint32_t& aHeight, uint32_t aImageWidth, uint32_t aImageHeight);

		static std::string				GetExtension				(const std::string& aPath);
		static std::string				GetDirectory				(const std::string& aPath);
		static std::string				GetFileName					(const std::string& aPath);

		static bool						FileExists					(const std::string& aPath);
		static bool						DirectoryExists				(const std::string& aPath);
		static uint32_t					FileSize					(const std::string& aPath);

		template<typename T>
		static bool						ListVolumes					(T& aOutput)
		{
#ifndef COMPLEX_VOLUMES
			aOutput.push_back("/");
			return true;
#else
			return PlatformHelpers::ListVolumes<T>(aOutput);			
#endif
		}


		template<typename T>
		static bool						ListDirectory				(const std::string& aPath, T& aOutput)
		{
#ifndef NO_READDIR
#ifndef S_ISDIR
#define S_ISDIR(a) ((a & S_IFMT) == S_IFDIR)
#endif
			DIR* dirhandle;
			struct dirent* item;
			
			if((dirhandle = opendir(aPath.c_str())))
			{
				while((item = readdir(dirhandle)))
				{
					struct stat statbuf;
					stat((aPath + item->d_name).c_str(), &statbuf);
				
					if(S_ISDIR(statbuf.st_mode) && (strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0))
					{
						continue;
					}

					aOutput.push_back(std::string(item->d_name) + (S_ISDIR(statbuf.st_mode) ? "/" : ""));
				}

				closedir(dirhandle);				
				
				return true;
			}
			else
			{
				return false;
			}
#else
			return PlatformHelpers::ListDirectory<T>(aPath, aOutput);
#endif
		}
};


