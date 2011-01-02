#ifndef UTILITY_H_
#define UTILITY_H_

class				Colors
{
	public:
		static const uint32_t			BackGround				= 0xD0D0D0C0;
		static const uint32_t			Border					= 0x604040FF;
		static const uint32_t			Normal 					= 0x202020FF;
		static const uint32_t			HighLight				= 0xA02020FF;
		static const uint32_t			SpecialNormal 			= 0x206020FF;
		static const uint32_t			SpecialHighLight		= 0x20A020FF;
};

class				Utility
{
	public:
		static std::string				GetExtension				(const std::string& aPath)
		{
			if(aPath.find(".") != std::string::npos)
			{
				return aPath.substr(aPath.rfind(".") + 1);
			}
			
			return "";
		}
	
		static bool						ListDirectory				(const std::string& aPath, std::vector<std::string>& aOutput)
		{
			DIR* dirhandle;
			struct dirent* item;
			
			if((dirhandle = opendir(aPath.c_str())))
			{
				while((item = readdir(dirhandle)))
				{
					struct stat statbuf;
					stat((aPath + item->d_name).c_str(), &statbuf);
				
					if((statbuf.st_mode & S_IFDIR) && (strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0))
					{
						continue;
					}

					aOutput.push_back(std::string(item->d_name) + ((statbuf.st_mode & S_IFDIR) ? "/" : ""));
				}

				closedir(dirhandle);				
				
				return true;
			}
			else
			{
				return false;
			}
		}
	
		static uint32_t					GetTicks					()
		{
			return SDL_GetTicks();
		}

		static void						Sleep						(uint32_t aMilliseconds)
		{
			SDL_Delay(aMilliseconds);
		}

		static std::vector<std::string>	StringToVector				(const std::string& aString, char aSeparate)
		{
			std::vector<std::string> result;
		
			char* string = (char*)malloc(aString.length() + 10);
			char sepp[2] = {0, 0};
			sepp[0] = aSeparate;
			strcpy(string, aString.c_str());
			
			char* value = strtok(string, sepp);
			while(value)
			{
				result.push_back(std::string(value));
				value = strtok(0, sepp);
			}
			
			free(string);
			return result;
		}
		
		static std::string				VectorToString				(std::vector<std::string>& aStrings, char aSeparate)
		{
			std::string output;
			
			for(std::vector<std::string>::iterator i = aStrings.begin(); i != aStrings.end(); i ++)
			{
				output += (*i) + std::string(1, aSeparate);
			}
			
			return output.substr(0, output.length() - 1);
		}

		static int32_t					Clamp						(int32_t aValue, int32_t aMin, int32_t aMax)
		{
			if(aValue < aMin)		return aMin;
			if(aValue > aMax)		return aMax;
			return aValue;
		}
		
};
class				PathBuild
{
	public:
									PathBuild					(const std::string& aBase) : Base(aBase){}
									
		std::string					Build						(const std::string& aSub){return Base + "/" + aSub;}
									
	protected:
		std::string					Base;
};

class				FastCounter
{
	public:
									FastCounter					(uint32_t aSpeed = 4, uint32_t aButton = ES_BUTTON_AUXRIGHT2);
									~FastCounter				();
								
		void						Tick						();
		bool						DrawNow						();
		uint32_t					GetFPS						();
								
	protected:
		uint32_t					Button;
		uint32_t					Speed;
		uint32_t					Counter;
		
		uint32_t					LastFPS;
		uint32_t					LastFPSTime;
		uint32_t					FrameCount;
};


#endif
