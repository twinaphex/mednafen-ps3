#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include <list>
#include "Platform.h"

class				Utility
{
	public:
		//Delete an object AND set it to null
		template<typename T> static void		Delete						(T*& aObject) {delete aObject; aObject = 0;}
		template<typename T> static void		DeleteContainer				(T& aContainer) {for(typename T::iterator i = aContainer.begin(); i != aContainer.end(); i ++) delete *i; aContainer.clear();}

		static uint32_t							GetTicks					() {return PlatformHelpers::GetTicks();}
		static void								Sleep						(uint32_t aMilliseconds) {return PlatformHelpers::Sleep(aMilliseconds);}

		static int32_t							Clamp						(int32_t aValue, int32_t aMin, int32_t aMax) {return std::min(std::max(aValue, aMin), aMax);}

		static void								StringToVector				(std::vector<std::string>& aOutput, const std::string& aString, char aSeparate);
		static std::string&						VectorToString				(std::string& aOutput, const std::vector<std::string>& aStrings, char aSeparate);

		static void*							AllocateExecutable			(uint32_t aSize);
		static void								FreeExecutable				(void* aData, uint32_t aSize);

		static char*							VAPrint						(char* aBuffer, uint32_t aLength, const char* aFormat, ...);
		static char*							VAPrintD					(const char* aFormat, ...);
		static void								CenterAndScale				(uint32_t& aX, uint32_t& aY, uint32_t& aWidth, uint32_t& aHeight, uint32_t aImageWidth, uint32_t aImageHeight);

		static std::string						GetExtension				(const std::string& aPath);
		static std::string						GetDirectory				(const std::string& aPath);
		static std::string						GetFileName					(const std::string& aPath);

		static bool								FileExists					(const std::string& aPath);
		static bool								DirectoryExists				(const std::string& aPath);
		static uint32_t							FileSize					(const std::string& aPath);

		static bool								ListVolumes					(std::list<std::string>& aOutput);
		static bool								ListDirectory				(const std::string& aPath, std::list<std::string>& aOutput);
};


