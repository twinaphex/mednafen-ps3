#pragma once

class				PlatformHelpers
{
	public:
		static uint32_t					GetTicks					();
		static void						Sleep						(uint32_t aMilliseconds);
		static void*					AllocateExecutable			(uint32_t aSize);
		static void						FreeExecutable				(void* aData, uint32_t aSize);

#ifdef __WIN32__
		static bool						ListDirectory				(const std::string& aPath, std::list<std::string>& aOutput);
		static bool						ListVolumes					(std::list<std::string>& aOutput);
#endif
};


