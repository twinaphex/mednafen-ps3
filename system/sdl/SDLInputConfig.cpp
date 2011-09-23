#include <es_system.h>
#include "SDLInputConfig.h"

namespace SDLInputConfig
{
	void					Load				(uint32_t* aData)
	{
		std::string output = LibES::BuildPath("inputconfig");
		FILE* configFile = fopen(output.c_str(), "r");

		if(!configFile)
		{
			Get(aData);
		}
		else
		{
			for(int i = 0; i != 14; i ++)
			{
				fscanf(configFile, "%08X\n", &aData[i]);
			}

			fclose(configFile);
		}
	}

	void					Dump				(uint32_t* aData)
	{
		std::string output = LibES::BuildPath("inputconfig");
		FILE* configFile = fopen(output.c_str(), "w");

		for(int i = 0; i != 14; i ++)
		{
			fprintf(configFile, "%08X\n", aData[i]);
		}

		fclose(configFile);
	}

	void					Get					(uint32_t* aData)
	{
		const char* buttons[] = {"Up", "Down", "Left", "Right", "Accept", "Cancel", "Shift", "Tab", "AuxLeft1", "AuxRight1", "AuxLeft2", "AuxRight2", "AuxLeft3", "AuxRight3"};

		for(int j = 0; j != 14; j ++)
		{
			SummerfaceButton button(Area(10, 30, 80, 10), buttons[j]);
			Summerface("InputWindow", &button, false).Do();
			aData[j] = button.GetButton();
		}

		Dump(aData);
	}
};

