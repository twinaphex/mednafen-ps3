#include <mednafen_includes.h>

namespace
{
	bool								AlphaSortC								(ListItem* a, ListItem* b)
	{
		if(a->GetText() == "none") return false;
		if(b->GetText() == "none") return true;

		return a->GetText() < b->GetText();
	}

	bool								InputSortC								(InputHandler::InputInfo a, InputHandler::InputInfo b)
	{
		if(a.ConfigOrder != b.ConfigOrder)
		{
			return a.ConfigOrder < b.ConfigOrder;
		}
		else
		{
			return a.Index < b.Index;
		}
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


									MednafenSettingButton::MednafenSettingButton	(const std::string& aInputName, const std::string& aImage) : Winterface("Configuring Inputs")
{
	Button = 0;
	InputName = aInputName;
	Image = aImage;

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

	Area range = es_video->GetClip();

	Texture* img = ImageManager::GetImage(Image);

	if(img)
	{
		uint32_t x = 2;
		uint32_t y = (range.Height / 2) + 2;
		uint32_t w = range.Width - 4;
		uint32_t h = (range.Height / 2) - 4;

		Utility::CenterAndScale(x, y, w, h, img->GetWidth(), img->GetHeight());

		es_video->PlaceTexture(img, x, y, w, h);
	}

	return false;
}

uint32_t							MednafenSettingButton::GetButton				()
{
	return Button;
}

								InputHandler::InputHandler				(MDFNGI* aGameInfo)
{
	GameInfo = aGameInfo;

	memset(ControllerBits, 0, sizeof(ControllerBits));

	ReadSettings();

	//Set the inputs
	//TODO: Separate pad type for each port
	for(int i = 0; i != GameInfo->InputInfo->InputPorts; i ++)
	{
		MDFNI_SetInput(i, "gamepad", &ControllerBits[i], 1);
	}

	//Run configure if all input values are zero
	for(int i = 0; i != Inputs.size(); i ++)
	{
		if(Inputs[i].Button != 0)
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
		memset(ControllerBits[p], 0, 128);

		for(int i = 0; i != Inputs.size(); i ++)
		{
			if((Inputs[i].Type == IDIT_BUTTON || Inputs[i].Type == IDIT_BUTTON_CAN_RAPID) && (es_input->ButtonPressed(p, Inputs[i].Button)))
			{
				int byte = Inputs[i].BitOffset / 8;
				int bit = Inputs[i].BitOffset & 7;
				ControllerBits[p][byte] |= 1 << bit;
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

	std::vector<InputInfo> inputs;
	BuildShifts(info, buttoncount, inputs);
	for(int j = 0; j != inputs.size(); j ++)
	{
		if(info[j].SettingName)
		{
			char buffer[2048];
			snprintf(buffer, 2048, "%s%sIMAGE", GameInfo->shortname, PadType.c_str());
			MednafenSettingButton button(info[inputs[j].Index].Name, buffer);
			button.Do();

			std::string settingname = std::string(GameInfo->shortname) + ".esinput." + PadType + "." + std::string(info[inputs[j].Index].SettingName);
			MDFNI_SetSettingUI(settingname.c_str(), button.GetButton());
		}
	}
}

void							InputHandler::ReadSettings			()
{
	PadType = MDFN_GetSettingS((std::string(GameInfo->shortname) + ".esinput.port1").c_str());

	uint32_t ButtonCount;
	const InputDeviceInputInfoStruct* info = GetGamepad(GameInfo->InputInfo, PadType.c_str(), ButtonCount);

	if(info == 0)
	{
		Inputs.clear();
		return;
	}

	BuildShifts(info, ButtonCount, Inputs);
	for(int j = 0; j != Inputs.size(); j ++)
	{
		if(info[Inputs[j].Index].SettingName)
		{
			std::string settingname = std::string(GameInfo->shortname) + ".esinput." + PadType + "." + std::string(info[Inputs[j].Index].SettingName);
			Inputs[j].Button = MDFN_GetSettingUI(settingname.c_str());
		}
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

			if(info)
			{
				for(int j = 0; j != buttoncount; j ++)
				{
					if(info[j].SettingName)
					{
						std::string settingname = std::string(MDFNSystems[i]->shortname) + ".esinput." + MDFNSystems[i]->InputInfo->Types[0].DeviceInfo[k].ShortName + "." + std::string(info[j].SettingName);
						//TODO: These strdups will never be freed, poor captive strdups
						MDFNSetting	thisinput = {strdup(settingname.c_str()), MDFNSF_NOFLAGS, "Input.", NULL, MDFNST_UINT, "0"};
						aSettings.push_back(thisinput);
					}
				}
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

void							InputHandler::BuildShifts				(const InputDeviceInputInfoStruct* aInfo, uint32_t aButtonCount, std::vector<InputInfo>& aInputs)
{
	//TODO: Support more input button types
	int byte = 0;
	int bit = 0;
	int found = 0;

	aInputs.clear();

	for(int j = 0; j != aButtonCount; j ++)
	{
		InputInfo ii;

		if(aInfo[j].Type == IDIT_BUTTON || aInfo[j].Type == IDIT_BUTTON_CAN_RAPID)
		{
			ii.BitOffset = byte * 8 + bit;
			if(++bit == 8)
			{
				bit = 0;
				byte ++;
			}
		}

		ii.Button = 0;
		ii.Type = aInfo[j].Type;
		ii.Index = j;
		ii.ConfigOrder = aInfo[j].ConfigOrder;
		aInputs.push_back(ii);
	}

	std::sort(aInputs.begin(), aInputs.end(), InputSortC);
}


