#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <map>
#include <string>

#include "SetText.h"

#if ENABLE_NLS

namespace
{
	char*									MOData;

	class									Compare
	{
		public:
			bool							operator()					(const char* aA, const char* aB)
			{
				return strcmp(aA, aB) < 0;
			}
	};

	struct									FileData
	{
		char*								Data;
		uint32_t							Length;
		bool								BigEndian;
	};

	uint32_t								Read32						(const char* aSource, bool aBigEndian)
	{
		const uint8_t* data = (const uint8_t*)aSource;

		if(aBigEndian)
		{
			return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | (data[3]);
		}
		else
		{
			return (data[0]) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
		}
	} 

	std::map<std::string, FileData>						FileCache;
	std::map<const char*, const char*, Compare>			Messages;
};

void										SETTEXT_CleanUp				()
{
	Messages.clear();
}

void										SETTEXT_SetMessageFile		(const char* aFileName)
{
	assert(aFileName);

	SETTEXT_CleanUp();

	if(FileCache.find(aFileName) == FileCache.end())
	{
		FILE* moFile = fopen(aFileName, "r");
		if(!moFile)
		{
			return;
		}

		FileData fileInfo;

		//Get file size
		fseek(moFile, 0, SEEK_END);
		fileInfo.Length = ftell(moFile);
		fseek(moFile, 0, SEEK_SET);
	
		//Read file, leave a zero byte at the end for some false security
		fileInfo.Data = new char[fileInfo.Length + 1];
		fread(fileInfo.Data, fileInfo.Length, 1, moFile);
		fclose(moFile);
		fileInfo.Data[fileInfo.Length] = 0;

		//Test file
		uint32_t magic = Read32(fileInfo.Data, false);
		if(magic == 0xDE120495)
		{
			fileInfo.BigEndian = true;
		}
		else if(magic == 0x950412DE)
		{
			fileInfo.BigEndian = false;
		}
		else
		{
			delete[] fileInfo.Data;
			SETTEXT_CleanUp();
			return;
		}

		//Map it
		FileCache[aFileName] = fileInfo;
	}

	FileData* file = &FileCache[aFileName];

	//Read strings
	char* origStrings = &file->Data[Read32(&file->Data[3 * 4], file->BigEndian)];
	char* tranStrings = &file->Data[Read32(&file->Data[4 * 4], file->BigEndian)];

	for(int i = 0; i != Read32(&file->Data[2 * 4], file->BigEndian); i ++)
	{
		uint32_t source = Read32(&origStrings[(i * 2 + 1) * 4], file->BigEndian);
		uint32_t target = Read32(&tranStrings[(i * 2 + 1) * 4], file->BigEndian);


		//No strings past the end of the file OK
		//TODO: Test that a string doesn't start in the file then terminate after the end!
		if(source >= file->Length || target >= file->Length)
		{
			SETTEXT_CleanUp();
			return;
		}

		Messages[&file->Data[source]] = &file->Data[target];
	}
}

char*										SETTEXT_GetText				(const char* aString)
{
	if(aString)
	{
		std::map<const char*, const char*, Compare>::const_iterator msg = Messages.find(aString);

		if(msg != Messages.end())
		{
			return (char*)msg->second;
		}
	}

	return (char*)aString;
}

#endif

