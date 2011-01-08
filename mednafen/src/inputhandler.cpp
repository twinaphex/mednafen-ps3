#include <mednafen_includes.h>

namespace
{
	bool								AlphaSortC								(ListItem* a, ListItem* b)
	{
		if(a->GetText() == "none") return false;
		if(b->GetText() == "none") return true;

		return a->GetText() < b->GetText();
	}
}

									MednafenInputItem::MednafenInputItem				(const std::string& aNiceName, const std::string& aRealName) : ListItem(aNiceName)
{
	RealName = aRealName;
}

std::string							MednafenInputItem::GetRealName					()
{
	return RealName;
}


									MednafenInputSelect::MednafenInputSelect		(const std::vector<std::string>& aInputNames, const std::vector<std::string>& aNiceNames) : WinterfaceList("Select Controller Type", true, 0)
{
	for(int i = 0; i != aInputNames.size(); i ++)
	{
		Items.push_back(new MednafenInputItem(aNiceNames[i], aInputNames[i]));
	}

	std::sort(Items.begin(), Items.end(), AlphaSortC);
}


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

	ReadSettings();

	for(int i = 0; i != GameInfo->InputInfo->InputPorts; i ++)
	{
		MDFNI_SetInput(i, PadType.c_str(), &ControllerBits[i], 4);
	}

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
	//TODO: Support more input types, more than two controllers
	for(int p = 0; p != GameInfo->InputInfo->InputPorts && p != 2; p ++)
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
	//Get Controller type
	if(GameInfo->InputInfo->Types[0].NumTypes > 1)
	{
		std::vector<std::string> inputtypes;
		std::vector<std::string> niceinputtypes;
		for(int i = 0; i != GameInfo->InputInfo->Types[0].NumTypes; i ++)
		{
			inputtypes.push_back(GameInfo->InputInfo->Types[0].DeviceInfo[i].ShortName);
			niceinputtypes.push_back(GameInfo->InputInfo->Types[0].DeviceInfo[i].FullName);
		}

		MednafenInputSelect controlselect(inputtypes, niceinputtypes);

		//TODO: Let it be canceled?
		do
		{
			controlselect.Do();
		} while(controlselect.WasCanceled());

		PadType = ((MednafenInputItem*)controlselect.GetSelected())->GetRealName();
	}
	else
	{
		PadType = GameInfo->InputInfo->Types[0].DeviceInfo[0].ShortName;
	}

	MDFNI_SetSetting((std::string(GameInfo->shortname) + ".esinput.port1").c_str(), PadType.c_str());

	//Get Buttons
	uint32_t buttoncount;
	const InputDeviceInputInfoStruct* info = GetGamepad(GameInfo->InputInfo, PadType.c_str(), buttoncount);

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

		std::string settingname = std::string(GameInfo->shortname) + ".esinput." + PadType + "." + std::string(info[ButtonOrder[j][0]].SettingName);
		MDFNI_SetSettingUI(settingname.c_str(), button.GetButton());
	}
}

void							InputHandler::ReadSettings			()
{
	PadType = MDFN_GetSettingS((std::string(GameInfo->shortname) + ".esinput.port1").c_str());

	const InputDeviceInputInfoStruct* info = GetGamepad(GameInfo->InputInfo, PadType.c_str(), ButtonCount);

	if(info == 0)
	{
		memset(Button, 0, sizeof(Button));
		return;
	}

	BuildShifts(info, ButtonCount, Button);

	for(int j = 0; j != ButtonCount; j ++)
	{
		std::string settingname = std::string(GameInfo->shortname) + ".esinput." + PadType + "." + std::string(info[Button[j][0]].SettingName);
		Button[j][0] = MDFN_GetSettingUI(settingname.c_str());
	}
}

void							InputHandler::GenerateSettings			(std::vector<MDFNSetting>& aSettings)
{
	for(int i = 0; i != MDFNSystems.size(); i ++)
	{
		for(int k = 0; k != MDFNSystems[i]->InputInfo->Types[0].NumTypes; k ++)
		{
			if(k == 0)
			{
				//TODO: Strdup will no be freed
				MDFNSetting thisinput = {strdup((std::string(MDFNSystems[i]->shortname) + ".esinput.port1").c_str()), MDFNSF_NOFLAGS, "Input.", NULL, MDFNST_STRING, "gamepad"};
				aSettings.push_back(thisinput);
			}

			//TODO: Support port expanders
			if(MDFNSystems[i]->InputInfo->Types[0].DeviceInfo[k].PortExpanderDeviceInfo)
			{
				continue;
			}

			uint32_t buttoncount;
			const InputDeviceInputInfoStruct* info = GetGamepad(MDFNSystems[i]->InputInfo, MDFNSystems[i]->InputInfo->Types[0].DeviceInfo[k].ShortName, buttoncount);

			if(info == 0 || !IsInputSupported(info, buttoncount))
			{
				continue;
			}

			for(int j = 0; j != buttoncount; j ++)
			{
				std::string settingname = std::string(MDFNSystems[i]->shortname) + ".esinput." + MDFNSystems[i]->InputInfo->Types[0].DeviceInfo[k].ShortName + "." + std::string(info[j].SettingName);
				//TODO: These strdups will never be freed, poor captive strdups
				MDFNSetting	thisinput = {strdup(settingname.c_str()), MDFNSF_NOFLAGS, "Input.", NULL, MDFNST_UINT, "0"};
				aSettings.push_back(thisinput);
			}
		}
	}
}


const InputDeviceInputInfoStruct*	InputHandler::GetGamepad				(const InputInfoStruct* aInfo, const char* aName, uint32_t& aInputCount)
{
	for(int i = 0; i != aInfo->Types[0].NumTypes; i ++)
	{
		if(strcmp(aInfo->Types[0].DeviceInfo[i].ShortName, aName) == 0)
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

bool							InputHandler::IsInputSupported			(const InputDeviceInputInfoStruct* aInfo, uint32_t aButtonCount)
{
	if(aButtonCount > 16 || aButtonCount == 0)
	{
		return false;
	}

	for(int i = 0; i != aButtonCount; i ++)
	{
		if(aInfo[i].Type != IDIT_BUTTON && aInfo[i].Type != IDIT_BUTTON_CAN_RAPID)
		{
			return false;
		}
	}

	return true;
}
