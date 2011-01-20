#include <mednafen_includes.h>

namespace
{
	bool								InputSortC								(InputHandler::InputInfo a, InputHandler::InputInfo b)
	{
		if(a.Data->ConfigOrder != b.Data->ConfigOrder)
		{
			return a.Data->ConfigOrder < b.Data->ConfigOrder;
		}
		else
		{
			return a.Index < b.Index;
		}
	}
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
			if((Inputs[i].Data->Type == IDIT_BUTTON || Inputs[i].Data->Type == IDIT_BUTTON_CAN_RAPID) && (es_input->ButtonPressed(p, Inputs[i].Button)))
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
	Summerface* sface = 0;

	//Get Controller type
	if(GameInfo->InputInfo->Types[0].NumTypes > 1)
	{
		SummerfaceLineList*	linelist = new SummerfaceLineList(Area(10, 10, 80, 20));

		for(int i = 0; i != GameInfo->InputInfo->Types[0].NumTypes; i ++)
		{
			SummerfaceItem* item = new SummerfaceItem(GameInfo->InputInfo->Types[0].DeviceInfo[i].FullName, "");
			item->Properties["REALNAME"] = GameInfo->InputInfo->Types[0].DeviceInfo[i].ShortName;
			linelist->AddItem(item);
		}

		sface = new Summerface("InputTypeSelect", linelist);
		sface->Do();

		PadType = linelist->GetSelected()->Properties["REALNAME"];
	}
	else
	{
		PadType = GameInfo->InputInfo->Types[0].DeviceInfo[0].ShortName;
	}

	MDFNI_SetSetting((std::string(GameInfo->shortname) + ".esinput.port1").c_str(), PadType.c_str());

	//Get Buttons
	std::vector<InputInfo> inputs;
	GetGamepad(GameInfo->InputInfo, PadType.c_str(), inputs);

	for(int j = 0; j != inputs.size(); j ++)
	{
		if(inputs[j].Data->SettingName)
		{
			uint32_t buttonID;
			SummerfaceLabel* button = new SummerfaceLabel(Area(10, 30, 80, 10), inputs[j].Data->Name);
			button->SetInputConduit(new SummerfaceStaticConduit(GetButton, &buttonID), true);

			if(sface)
			{
				sface->AddWindow("InputWindow", button);
			}
			else
			{
				sface = new Summerface("InputWindow", button);
			}
			sface->Do();

			std::string settingname = std::string(GameInfo->shortname) + ".esinput." + PadType + "." + std::string(inputs[j].Data->SettingName);
			MDFNI_SetSettingUI(settingname.c_str(), buttonID);

			sface->RemoveWindow("InputWindow", true);
		}
	}

	if(sface)
	{
		delete sface;
	}
}

void							InputHandler::ReadSettings			()
{
	PadType = MDFN_GetSettingS((std::string(GameInfo->shortname) + ".esinput.port1").c_str());

	GetGamepad(GameInfo->InputInfo, PadType.c_str(), Inputs);

	for(int j = 0; j != Inputs.size(); j ++)
	{
		if(Inputs[j].Data->SettingName)
		{
			std::string settingname = std::string(GameInfo->shortname) + ".esinput." + PadType + "." + std::string(Inputs[j].Data->SettingName);
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

			std::vector<InputInfo> inputs;
			GetGamepad(MDFNSystems[i]->InputInfo, MDFNSystems[i]->InputInfo->Types[0].DeviceInfo[k].ShortName, inputs);

			for(int j = 0; j != inputs.size(); j ++)
			{
				if(inputs[j].Data->SettingName)
				{
					std::string settingname = std::string(MDFNSystems[i]->shortname) + ".esinput." + MDFNSystems[i]->InputInfo->Types[0].DeviceInfo[k].ShortName + "." + std::string(inputs[j].Data->SettingName);
					//TODO: These strdups will never be freed, poor captive strdups
					MDFNSetting	thisinput = {strdup(settingname.c_str()), MDFNSF_NOFLAGS, "Input.", NULL, MDFNST_UINT, "0"};
					aSettings.push_back(thisinput);
				}
			}
		}
	}
}


void							InputHandler::GetGamepad				(const InputInfoStruct* aInfo, const char* aName, std::vector<InputInfo>& aInputs)
{
	const InputDeviceInputInfoStruct* inputinfo = 0;
	uint32_t buttoncount = 0;

	for(int i = 0; i != aInfo->Types[0].NumTypes; i ++)
	{
		if(strcmp(aInfo->Types[0].DeviceInfo[i].ShortName, aName) == 0)
		{
			buttoncount = aInfo->Types[0].DeviceInfo[i].NumInputs;
			inputinfo = aInfo->Types[0].DeviceInfo[i].IDII;
		}
	}

	aInputs.clear();

	if(inputinfo)
	{
		int byte = 0;
		int bit = 0;
		int found = 0;

		for(int j = 0; j != buttoncount; j ++)
		{
			InputInfo ii;
			memset(&ii, 0, sizeof(ii));

			if(inputinfo[j].Type == IDIT_BUTTON || inputinfo[j].Type == IDIT_BUTTON_CAN_RAPID)
			{
				ii.BitOffset = byte * 8 + bit;
				if(++bit == 8)
				{
					bit = 0;
					byte ++;
				}
			}

			ii.Index = j;
			ii.Data = &inputinfo[j];
			aInputs.push_back(ii);
		}

		std::sort(aInputs.begin(), aInputs.end(), InputSortC);
	}
}

bool							InputHandler::GetButton					(void* aUserData, Summerface* aInterface, const std::string& aWindow)
{
	static bool gotbutton = true;

	if(gotbutton && es_input->GetAnyButton(0) != 0xFFFFFFFF)
	{
		return false;
	}

	gotbutton = false;

	uint32_t* button = (uint32_t*)aUserData;
	button[0] = es_input->GetAnyButton(0);
	gotbutton = (button[0] != 0xFFFFFFFF) ? true : false;
	return gotbutton;
}


