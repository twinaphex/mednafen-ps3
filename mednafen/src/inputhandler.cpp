#include <mednafen_includes.h>

									MednafenSettingButton::MednafenSettingButton	(std::string aInputName) : Winterface("Configuring Inputs")
{
	Button = 0;
	InputName = aInputName;
	
	SideItems.push_back(new ListItem("Press Input Button", FontManager::GetSmallFont()));
}
		
bool								MednafenSettingButton::Input					()
{
	const MDFNSetting_EnumList* enums = InputHandler::Buttons;
	int32_t result = 0;
	
	while(enums->string)
	{
		if(PS3Input::ButtonDown(0, enums->number))
		{
			Button = result;
			return true;
		}
		
		result ++;
		enums ++;
	}

	return false;
}

bool								MednafenSettingButton::DrawLeft					()
{
	FontManager::GetBigFont()->PutString("Waiting for input", 40, 40, Colors::Normal);
	FontManager::GetBigFont()->PutString(InputName.c_str(), 40, 40 + FontManager::GetBigFont()->GetHeight() * 2, Colors::Normal);
	return false;
}
		
std::string							MednafenSettingButton::GetButton				()
{
	return InputHandler::Buttons[Button].string;
}

								InputHandler::InputHandler				(MDFNGI* aGameInfo)
{
	GameInfo = aGameInfo;

	ButtonCount = 0;
	memset(ControllerBits, 0, sizeof(ControllerBits));
	memset(PS3Button, 0, sizeof(PS3Button));
	
	for(int i = 0; i != GameInfo->InputInfo->InputPorts; i ++)
	{
		MDFNI_SetInput(i, "gamepad", &ControllerBits[i], 4);
	}
	
	ReadSettings();
	
	for(int i = 0; i != ButtonCount; i ++)
	{
		if(PS3Button[i][0] != 0)
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
			if(PS3Input::ButtonPressed(p, PS3Button[i][0]))
			{
				ControllerBits[p] |= PS3Button[i][1];
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
	
		std::string settingname = std::string(GameInfo->shortname) + ".ps3input." + std::string(info[ButtonOrder[j][0]].SettingName);
		MDFNI_SetSetting(settingname.c_str(), button.GetButton().c_str());
	}
}

void							InputHandler::ReadSettings			()
{
	const InputDeviceInputInfoStruct* info = GetGamepad(GameInfo->InputInfo, ButtonCount);
		
	if(info == 0)
	{
		memset(PS3Button, 0, sizeof(PS3Button));
		return;
	}
		
	BuildShifts(info, ButtonCount, PS3Button);
		
	for(int j = 0; j != ButtonCount; j ++)
	{
		std::string settingname = std::string(GameInfo->shortname) + ".ps3input." + std::string(info[PS3Button[j][0]].SettingName);
		PS3Button[j][0] = MDFN_GetSettingUI(settingname.c_str());
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
			std::string settingname = std::string(MDFNSystems[i]->shortname) + ".ps3input." + std::string(info[ButtonOrder[j][0]].SettingName);
			//TODO: These strdups will never be freed, poor captive strdups
			MDFNSetting	thisinput = {strdup(settingname.c_str()), MDFNSF_NOFLAGS, "Input.", NULL, MDFNST_ENUM, "START", 0, 0, 0, 0, Buttons};
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
				aOrder[found][1] = (j < 8) ? 1 << (24 + j) : 1 << (16 + (j - 8));
				found ++;
			}
		}
	}
}

MDFNSetting_EnumList			InputHandler::Buttons[] = 
{
	{"START",		0,	"Start"},
	{"SELECT",		1,	"Select"},
	{"SQUARE",		2,	"Square"},
	{"CROSS",		3,	"Cross"},		
	{"TRIANGLE",	4,	"Triangle"},
	{"CIRCLE", 		5,	"Circle"},		
	{"UP", 			6,	"Up"},
	{"DOWN", 		7,	"Down"},		
	{"LEFT", 		8,	"Left"},		
	{"RIGHT", 		9,	"Right"},		
	{"L1", 			10, "L1"},		
	{"R1", 			13, "R1"},
	{0, 			-1,	0}
};

