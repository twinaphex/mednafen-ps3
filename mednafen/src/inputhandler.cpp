#include <mednafen_includes.h>

									MednafenSettingButton::MednafenSettingButton	(std::string aInputName) : Winterface("Configuring Inputs")
{
	Button = 0;
	InputName = aInputName;
	
	SideItems.push_back(new ListItem("Press Input Button", FontManager::GetSmallFont()));
}
		
bool								MednafenSettingButton::Input					()
{
	static bool gotbutton = true;

	if(gotbutton && es_input->GetAnyButton(0) != 0xFFFFFFFF)
	{
		return false;
	}
	
	gotbutton = false;

	Button = es_input->GetAnyButton(0);
	gotbutton = (Button != 0xFFFFFFFF) ? true : false;
	return gotbutton;
}

bool								MednafenSettingButton::DrawLeft					()
{
	FontManager::GetBigFont()->PutString("Waiting for input", 40, 40, Colors::Normal);
	FontManager::GetBigFont()->PutString(InputName.c_str(), 40, 40 + FontManager::GetBigFont()->GetHeight() * 2, Colors::Normal);
	return false;
}
		
uint32_t							MednafenSettingButton::GetButton				()
{
	return Button;
}

								InputHandler::InputHandler				(MDFNGI* aGameInfo)
{
	GameInfo = aGameInfo;

	ButtonCount = 0;
	memset(ControllerBits, 0, sizeof(ControllerBits));
	memset(Button, 0, sizeof(Button));
	
	for(int i = 0; i != GameInfo->InputInfo->InputPorts; i ++)
	{
		MDFNI_SetInput(i, "gamepad", &ControllerBits[i], 4);
	}
	
	ReadSettings();
	
	for(int i = 0; i != ButtonCount; i ++)
	{
		if(Button[i][0] != 0)
		{
			return;
		}
	}
	
	Configure();
	ReadSettings();
}

								InputHandler::~InputHandler				()
{

}

void							InputHandler::Process					()
{
	//TODO: Support more input types
	for(int p = 0; p != GameInfo->InputInfo->InputPorts; p ++)	
	{
		ControllerBits[p] = 0;

		for(int i = 0; i != ButtonCount; i ++)
		{
			if(es_input->ButtonPressed(p, Button[i][0]))
			{
				ControllerBits[p] |= Button[i][1];
			}
		}
	}
}

void							InputHandler::Configure				()
{
	uint32_t buttoncount;
	const InputDeviceInputInfoStruct* info = GetGamepad(GameInfo->InputInfo, buttoncount);
	
	if(info == 0)
	{
		return;
	}
	
	uint32_t ButtonOrder[32][2];
	BuildShifts(info, buttoncount, ButtonOrder);
	
	for(int j = 0; j != buttoncount; j ++)
	{
		MednafenSettingButton button(info[ButtonOrder[j][0]].Name);
		button.Do();
	
		std::string settingname = std::string(GameInfo->shortname) + ".esinput." + std::string(info[ButtonOrder[j][0]].SettingName);
		MDFNI_SetSettingUI(settingname.c_str(), button.GetButton());
	}
}

void							InputHandler::ReadSettings			()
{
	const InputDeviceInputInfoStruct* info = GetGamepad(GameInfo->InputInfo, ButtonCount);
		
	if(info == 0)
	{
		memset(Button, 0, sizeof(Button));
		return;
	}
		
	BuildShifts(info, ButtonCount, Button);
		
	for(int j = 0; j != ButtonCount; j ++)
	{
		std::string settingname = std::string(GameInfo->shortname) + ".esinput." + std::string(info[Button[j][0]].SettingName);
		Button[j][0] = MDFN_GetSettingUI(settingname.c_str());
	}
}

void							InputHandler::GenerateSettings			(std::vector<MDFNSetting>& aSettings)
{
	for(int i = 0; i != MDFNSystems.size(); i ++)
	{
		uint32_t buttoncount;
		const InputDeviceInputInfoStruct* info = GetGamepad(MDFNSystems[i]->InputInfo, buttoncount);
		
		if(info == 0)
		{
			continue;
		}
		
		uint32_t ButtonOrder[32][2];
		BuildShifts(info, buttoncount, ButtonOrder);
		
		for(int j = 0; j != buttoncount; j ++)
		{
			std::string settingname = std::string(MDFNSystems[i]->shortname) + ".esinput." + std::string(info[ButtonOrder[j][0]].SettingName);
			//TODO: These strdups will never be freed, poor captive strdups
			MDFNSetting	thisinput = {strdup(settingname.c_str()), MDFNSF_NOFLAGS, "Input.", NULL, MDFNST_UINT, "0"};
			aSettings.push_back(thisinput);
		}
	}
}


const InputDeviceInputInfoStruct*	InputHandler::GetGamepad				(const InputInfoStruct* aInfo, uint32_t& aInputCount)
{
	//TODO: Support other controls
	for(int i = 0; i != aInfo->Types[0].NumTypes; i ++)
	{
		if(strcmp(aInfo->Types[0].DeviceInfo[i].ShortName, "gamepad") == 0)
		{
			aInputCount = aInfo->Types[0].DeviceInfo[i].NumInputs;
			return aInfo->Types[0].DeviceInfo[i].IDII;
		}
	}
	
	aInputCount = 0;
	return 0;
}

void							InputHandler::BuildShifts				(const InputDeviceInputInfoStruct* aInfo, uint32_t aButtonCount, uint32_t aOrder[32][2])
{
	//TODO: Support more input button types
	int bitstart = 24;
	int found = 0;

	memset(aOrder, 0, sizeof(aOrder));
		
	for(int i = 0; i != 0xFFFFFFFF && found != aButtonCount; i ++)
	{
		for(int j = 0; j != aButtonCount; j ++)
		{
			if(aInfo[j].ConfigOrder == i)
			{
				aOrder[found][0] = j;			
#ifdef MSB_FIRST
				aOrder[found][1] = (j < 8) ? 1 << (24 + j) : 1 << (16 + (j - 8));
#else
				aOrder[found][1] = 1 << j;
#endif
				found ++;
			}
		}
	}
}
