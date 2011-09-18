#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "Utility.h"

void						Utility::StringToVector				(std::vector<std::string>& aOutput, const std::string& aString, char aSeparate)
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
		
std::string&				Utility::VectorToString				(std::string& aOutput, const std::vector<std::string>& aStrings, char aSeparate)
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

void*						Utility::AllocateExecutable			(uint32_t aSize)
{
#ifndef NXSUPPORT
	return malloc(aSize);
#else
	return PlatformHelpers::AllocateExecutable(aSize);
#endif
}

void						Utility::FreeExecutable				(void* aData, uint32_t aSize)
{
#ifndef NXSUPPORT
	free(aData);
#else
	PlatformHelpers::FreeExecutable(aData, aSize);
#endif
}

char*						Utility::VAPrint					(char* aBuffer, uint32_t aLength, const char* aFormat, ...)
{
	assert(aFormat && aBuffer && aLength);

	va_list args;
	va_start(args, aFormat);
	vsnprintf(aBuffer, aLength, aFormat, args);
	va_end(args);

	return aBuffer;
}

char*						Utility::VAPrintD					(const char* aFormat, ...)
{
	assert(aFormat);

	char buffer[1024];

	va_list args;
	va_start(args, aFormat);
	vsnprintf(buffer, 1023, aFormat, args);
	va_end(args);

	return strdup(buffer);
}


void						Utility::CenterAndScale				(uint32_t& aX, uint32_t& aY, uint32_t& aWidth, uint32_t& aHeight, uint32_t aImageWidth, uint32_t aImageHeight)
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

std::string					Utility::GetExtension				(const std::string& aPath)
{
	if(aPath.find(".") != std::string::npos)
	{
		return aPath.substr(aPath.rfind(".") + 1);
	}
	
	return "";
}

std::string					Utility::GetDirectory				(const std::string& aPath)
{
	if(aPath.find("/") != std::string::npos)
	{
		return aPath.substr(0, aPath.rfind("/") + 1);
	}

	return "";
}

std::string					Utility::GetFileName				(const std::string& aPath)
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

bool						Utility::FileExists					(const std::string& aPath)
{
#ifndef NO_STAT
	struct stat statbuf;
	return ((0 == stat(aPath.c_str(), &statbuf)) && (statbuf.st_mode & S_IFREG));
#else
	return PlatformHelpers::FileExists(aPath);
#endif
}

bool						Utility::DirectoryExists			(const std::string& aPath)
{
#ifndef NO_STAT
	struct stat statbuf;
	return ((0 == stat(aPath.c_str(), &statbuf)) && (statbuf.st_mode & S_IFDIR));
#else
	return PlatformHelpers::FileExists(aPath);
#endif
}

uint32_t					Utility::FileSize					(const std::string& aPath)
{
#ifndef NO_STAT
	struct stat statbuf;
	return ((0 == stat(aPath.c_str(), &statbuf)) && (statbuf.st_mode & S_IFREG)) ? statbuf.st_size : 0;
#else
	return PlatformHelpers::FileSize(aPath);
#endif
}

