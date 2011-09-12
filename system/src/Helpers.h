#pragma once

class				Utility
{
	public:
		static char*					VAPrint						(char* aBuffer, uint32_t aLength, const char* aFormat, ...)
		{
			assert(aFormat && aBuffer && aLength);

			va_list args;
			va_start(args, aFormat);
			vsnprintf(aBuffer, aLength, aFormat, args);
			va_end(args);

			return aBuffer;
		}

		static char*					VAPrintD					(const char* aFormat, ...)
		{
			assert(aFormat);

			char buffer[1024];

			va_list args;
			va_start(args, aFormat);
			vsnprintf(buffer, 1023, aFormat, args);
			va_end(args);

			return strdup(buffer);
		}


		static void						CenterAndScale				(uint32_t& aX, uint32_t& aY, uint32_t& aWidth, uint32_t& aHeight, uint32_t aImageWidth, uint32_t aImageHeight)
		{
			float outputaspect = (float)aImageWidth / (float)aImageHeight;
			float imageaspect = (float)aWidth / (float)aHeight;

			if(outputaspect > imageaspect)
			{
				uint32_t oldheight = aHeight;
				aHeight = (uint32_t)((float)aWidth / outputaspect);
				aY += (oldheight - aHeight) / 2;
			}
			else
			{
				uint32_t oldwidth = aWidth;
				aWidth = (uint32_t)((float)aHeight * outputaspect);
				aX += (oldwidth - aWidth) / 2;
			}
		}

		static std::string				GetExtension				(const std::string& aPath)
		{
			if(aPath.find(".") != std::string::npos)
			{
				return aPath.substr(aPath.rfind(".") + 1);
			}
			
			return "";
		}

		static std::string				GetDirectory				(const std::string& aPath)
		{
			if(aPath.find("/") != std::string::npos)
			{
				return aPath.substr(0, aPath.rfind("/") + 1);
			}

			return "";
		}

		static std::string				GetFileName					(const std::string& aPath)
		{
			std::string out = aPath;

			if(aPath.find("/") != std::string::npos)
			{
				out = aPath.substr(aPath.rfind("/") + 1);
			}

			if(out.find(".") != std::string::npos)
			{
				out = out.substr(0, out.rfind("."));
			}

			return out;
		}


		static bool						FileExists					(const std::string& aPath)
		{
#ifndef NO_STAT
			struct stat statbuf;

			if(0 == stat(aPath.c_str(), &statbuf))
			{
				return (statbuf.st_mode & S_IFREG);
			}
			else
			{
				return false;
			}
#else
			return PlatformHelpers::FileExists(aPath);
#endif
		}

		static bool						DirectoryExists				(const std::string& aPath)
		{
#ifndef NO_STAT
			struct stat statbuf;

			if(0 == stat(aPath.c_str(), &statbuf))
			{
				return (statbuf.st_mode & S_IFDIR);
			}
			else
			{
				return false;
			}
#else
			return PlatformHelpers::FileExists(aPath);
#endif
		}

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
	
		static uint32_t					GetTicks					()
		{
			return PlatformHelpers::GetTicks();
		}

		static void						Sleep						(uint32_t aMilliseconds)
		{
			return PlatformHelpers::Sleep(aMilliseconds);
		}

		static void						StringToVector				(std::vector<std::string>& aOutput, const std::string& aString, char aSeparate)
		{
			char* string = (char*)malloc(aString.length() + 10);
			char sepp[2] = {0, 0};
			sepp[0] = aSeparate;
			strcpy(string, aString.c_str());
			
			char* value = strtok(string, sepp);
			while(value)
			{
				aOutput.push_back(std::string(value));
				value = strtok(0, sepp);
			}
			
			free(string);
		}
		
		static std::string&				VectorToString				(std::string& aOutput, const std::vector<std::string>& aStrings, char aSeparate)
		{
			if(aStrings.size())
			{
				for(std::vector<std::string>::const_iterator i = aStrings.begin(); i != aStrings.end(); i ++)
				{
					aOutput += (*i) + std::string(1, aSeparate);
				}

				aOutput.erase(aOutput.length() - 1);
			}

			return aOutput;
		}

		static int32_t					Clamp						(int32_t aValue, int32_t aMin, int32_t aMax)
		{
			if(aValue < aMin)		return aMin;
			if(aValue > aMax)		return aMax;
			return aValue;
		}
		
};


