#include <stdio.h>
#include <stdint.h>
#include <map>
#include <string>

#include "SetText.h"

#if ENABLE_NLS

namespace
{
	char*									MOData;
	std::map<std::string, char*>			Messages;
};

void										SETTEXT_CleanUp				()
{
	Messages.clear();

	delete MOData;
	MOData = 0;
}

void										SETTEXT_SetMessageFile		(const char* aFileName)
{
	SETTEXT_CleanUp();

	FILE* moFile = fopen(aFileName, "r");
	if(!moFile)
	{
		return;
	}

	//Get file size
	fseek(moFile, 0, SEEK_END);
	uint32_t len = ftell(moFile);
	fseek(moFile, 0, SEEK_SET);
	
	//Read file, leave a zero byte at the end for some false security
	MOData = new char[len + 1];
	uint32_t* data32 = (uint32_t*)MOData;
	fread(MOData, len, 1, moFile);
	fclose(moFile);
	MOData[len] = 0;

	//Test file
	if(!(data32[0] == 0x950412DE || data32[0] == 0xDE120495))
	{
		SETTEXT_CleanUp();
		return;
	}

	//Read strings
	uint32_t* origStrings = (uint32_t*)&MOData[data32[3]];
	uint32_t* tranStrings = (uint32_t*)&MOData[data32[4]];

	for(int i = 0; i != data32[2]; i ++)
	{
		//No strings past the end of the file OK
		//TODO: Test that a string doesn't start in the file then terminate after the end!
		if(origStrings[i * 2 + 1] > len || tranStrings[i * 2 + 1] > len)
		{
			SETTEXT_CleanUp();
			return;
		}

		Messages[&MOData[origStrings[i * 2 + 1]]] = &MOData[tranStrings[i * 2 + 1]];
	}
}

char*										SETTEXT_GetText				(const char* aString)
{
	std::map<std::string, char*>::const_iterator msg = Messages.find(aString);

	if(msg != Messages.end())
	{
		return msg->second;
	}

	return (char*)aString;
}

#endif

