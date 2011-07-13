#include <es_system.h>

namespace SDLInputConfig
{
	bool					Running = false;

	void					Load				(uint32_t* aData)
	{
		std::string output = es_paths->Build("inputconfig");
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
		std::string output = es_paths->Build("inputconfig");
		FILE* configFile = fopen(output.c_str(), "w");

		for(int i = 0; i != 14; i ++)
		{
			fprintf(configFile, "%08X\n", aData[i]);
		}

		fclose(configFile);
	}

	int						GetButton			(void* aUserData, Summerface_Ptr aInterface, const std::string& aWindow, uint32_t aButton)
	{
		static bool gotbutton = true;

		if(gotbutton && ESInput::GetAnyButton(0) != 0xFFFFFFFF)
		{
			return 0;
		}

		gotbutton = false;

		uint32_t* button = (uint32_t*)aUserData;
		button[0] = ESInput::GetAnyButton(0);
		gotbutton = (button[0] != 0xFFFFFFFF) ? true : false;
		return gotbutton ? -1 : 0;
	}

	void					Get					(uint32_t* aData)
	{
		if(Running)
		{
			return;
		}

		Running = true;

		const char* buttons[] = {"Up", "Down", "Left", "Right", "Accept", "Cancel", "Shift", "Tab", "AuxLeft1", "AuxRight1", "AuxLeft2", "AuxRight2", "AuxLeft3", "AuxRight3"};

		uint32_t buttonID;
		SummerfaceLabel_Ptr button = smartptr::make_shared<SummerfaceLabel>(Area(10, 30, 80, 10), "");

		Summerface_Ptr sface = Summerface::Create("InputWindow", button);
		sface->AttachConduit(smartptr::make_shared<SummerfaceStaticConduit>(GetButton, &buttonID));
		sface->SetInputWait(false);

		for(int j = 0; j != 14; j ++)
		{
			button->SetMessage("Press button for [%s]", buttons[j]);
			sface->Do();

			aData[j] = buttonID;
		}

		Dump(aData);

		Running = false;
	}
};
