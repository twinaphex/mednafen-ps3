#include <mednafen_includes.h>

namespace
{
	bool								InputSortC								(InputHandler::InputInfo a, InputHandler::InputInfo b)
	{
		//Sort first by the ConfigOrder hint
		if(a.Data->ConfigOrder != b.Data->ConfigOrder)
		{
			return a.Data->ConfigOrder < b.Data->ConfigOrder;
		}
		//If equal use the overall index
		else
		{
			return a.Index < b.Index;
		}
	}
}

								InputHandler::InputHandler				(MDFNGI* aGameInfo) :
	GameInfo(aGameInfo),
	RapidCount(0)
{
	memset(ControllerBits, 0, sizeof(ControllerBits));

	ReadSettings();

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

void							InputHandler::Process					()
{
	//Update platform dependent input
	es_input->Refresh();

	//TODO: Support more input types, more than two controllers
	for(int p = 0; p != GameInfo->InputInfo->InputPorts && p != 2; p ++)
	{
		//Clear this controller
		memset(ControllerBits[p], 0, 128);

		//Update all of the buttons
		for(int i = 0; i != Inputs.size(); i ++)
		{
			//Calculate offset in ControllerBits
			int byte = Inputs[i].BitOffset / 8;
			int bit = Inputs[i].BitOffset & 7;

			//Get any non rapid press
			if((Inputs[i].Data->Type == IDIT_BUTTON || Inputs[i].Data->Type == IDIT_BUTTON_CAN_RAPID) && (es_input->ButtonPressed(p, Inputs[i].Button)))
			{
				ControllerBits[p][byte] |= 1 << bit;
			}

			//Get any rapid press
			if((Inputs[i].Data->Type == IDIT_BUTTON_CAN_RAPID) && (es_input->ButtonPressed(p, Inputs[i].RapidButton) && (RapidCount == 0)))
			{
				ControllerBits[p][byte] |= 1 << bit;
			}
		}
	}

	//Only toggle rapid counts every few frames, otherwise they may be missed
	RapidCount = RapidCount ? RapidCount - 1 : 3;
}

void							InputHandler::Configure				()
{
	Summerface_Ptr sface;

	//Get Controller type
	if(GameInfo->InputInfo->Types[0].NumTypes > 1)
	{
		//More than one type, run a list to choose
		SummerfaceList_Ptr linelist = boost::make_shared<SummerfaceList>(Area(10, 10, 80, 20));
		linelist->SetView(boost::make_shared<AnchoredListView>(linelist));

		for(int i = 0; i != GameInfo->InputInfo->Types[0].NumTypes; i ++)
		{
			SummerfaceItem_Ptr item = boost::make_shared<SummerfaceItem>(GameInfo->InputInfo->Types[0].DeviceInfo[i].FullName, "");
			item->Properties["REALNAME"] = GameInfo->InputInfo->Types[0].DeviceInfo[i].ShortName;
			linelist->AddItem(item);
		}

		sface = Summerface::Create("Categories", linelist);
		sface->Do();

		PadType = linelist->GetSelected()->Properties["REALNAME"];
	}
	else
	{
		//Only one type, skip the list
		PadType = GameInfo->InputInfo->Types[0].DeviceInfo[0].ShortName;
	}

	//Stash the pad type in the settings file
	MDFNI_SetSetting((std::string(GameInfo->shortname) + ".esinput.port1").c_str(), PadType.c_str());

	//Get Buttons
	std::vector<InputInfo> inputs;
	GetGamepad(GameInfo->InputInfo, PadType.c_str(), inputs);

	uint32_t buttonID;

	//Create the window to receive input
	SummerfaceLabel_Ptr button = boost::make_shared<SummerfaceLabel>(Area(10, 30, 80, 10), "");
	button->SetInputConduit(boost::make_shared<SummerfaceStaticConduit>(GetButton, &buttonID));

	//Add the window to any existing summerface, or create a new one if needed 
	if(sface)
	{
		sface->AddWindow("InputWindow", button);
	}
	else
	{
		sface = Summerface::Create("InputWindow", button);
	}

	//Attach any available layout images
	std::string imagename = std::string(GameInfo->shortname) + PadType + "IMAGE";
	if(ImageManager::GetImage(imagename))
	{
		sface->AddWindow("InputImage", boost::make_shared<SummerfaceImage>(Area(10, 50, 80, 40), imagename));
		sface->SetActiveWindow("InputWindow");	//Make sure the button window has the input focus
	}

	//Grab all of the buttons
	for(int j = 0; j != inputs.size(); j ++)
	{
		//Skip any whose setting name is null? (Was this REALLY needed?)
		if(inputs[j].Data->SettingName)
		{
			//Prep and run the interface
			button->SetMessage("Press button for [%s]", inputs[j].Data->Name);
			sface->Do();

			//Put the result in the settings file
			std::string settingname = std::string(GameInfo->shortname) + ".esinput." + PadType + "." + std::string(inputs[j].Data->SettingName);
			MDFNI_SetSettingUI(settingname.c_str(), buttonID);

			//If this button has the rapid flag set, do it again for the rapid version
			if(inputs[j].Data->Type == IDIT_BUTTON_CAN_RAPID)
			{
				button->SetMessage("Press button for [Rapid %s]", inputs[j].Data->Name);
				sface->Do();

				settingname += "_rapid";
				MDFNI_SetSettingUI(settingname.c_str(), buttonID);
			}
		}
	}
}

void							InputHandler::ReadSettings			()
{
	//Get the contoller type, and it's details
	PadType = MDFN_GetSettingS((std::string(GameInfo->shortname) + ".esinput.port1").c_str());
	GetGamepad(GameInfo->InputInfo, PadType.c_str(), Inputs);

	//Set the input types and bits to the mednafen core
	//TODO: Separate pad type for each port
	for(int i = 0; i != GameInfo->InputInfo->InputPorts; i ++)
	{
		MDFNI_SetInput(i, PadType.c_str(), &ControllerBits[i], 2);
	}

	//Get the button mapping from the settings file
	for(int j = 0; j != Inputs.size(); j ++)
	{
		//Skip any whose setting name is NULL
		if(Inputs[j].Data->SettingName)
		{
			//Fetch the setting
			std::string settingname = std::string(GameInfo->shortname) + ".esinput." + PadType + "." + std::string(Inputs[j].Data->SettingName);
			Inputs[j].Button = MDFN_GetSettingUI(settingname.c_str());

			//Fetch any rapid button mapping
			if(Inputs[j].Data->Type == IDIT_BUTTON_CAN_RAPID)
			{
				settingname += "_rapid";
				Inputs[j].RapidButton = MDFN_GetSettingUI(settingname.c_str());
			}
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

					if(inputs[j].Data->Type == IDIT_BUTTON_CAN_RAPID)
					{
						settingname += "_rapid";
						//TODO: These strdups will never be freed, poor captive strdups
						MDFNSetting	thisinput = {strdup(settingname.c_str()), MDFNSF_NOFLAGS, "Input.", NULL, MDFNST_UINT, "0"};
						aSettings.push_back(thisinput);
					}
				}
			}
		}
	}
}


void							InputHandler::GetGamepad				(const InputInfoStruct* aInfo, const char* aName, std::vector<InputInfo>& aInputs)
{
	const InputDeviceInputInfoStruct* inputinfo = 0;
	uint32_t buttoncount = 0;

	//Search the info for an entry with type aName and get some details
	for(int i = 0; i != aInfo->Types[0].NumTypes; i ++)
	{
		if(strcmp(aInfo->Types[0].DeviceInfo[i].ShortName, aName) == 0)
		{
			buttoncount = aInfo->Types[0].DeviceInfo[i].NumInputs;
			inputinfo = aInfo->Types[0].DeviceInfo[i].IDII;
		}
	}

	//Clear the input vector
	aInputs.clear();

	//If we found a controller, get its details
	if(inputinfo)
	{
		int byte = 0;
		int bit = 0;
		int found = 0;

		//Get every button
		for(int j = 0; j != buttoncount; j ++)
		{
			InputInfo ii;
			memset(&ii, 0, sizeof(ii));

			//Only support buttons and rapid buttons
			//TODO: Support other types of inputs
			if(inputinfo[j].Type == IDIT_BUTTON || inputinfo[j].Type == IDIT_BUTTON_CAN_RAPID)
			{
				//Mednafen has the most convoluted input scheme. 
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

bool							InputHandler::GetButton					(void* aUserData, Summerface_Ptr aInterface, const std::string& aWindow)
{
	//Conduit for config interface
	static bool gotbutton = true;

	//Don't continue until all buttons are released
	if(gotbutton && es_input->GetAnyButton(0) != 0xFFFFFFFF)
	{
		return false;
	}

	//Note that no buttons are pressed
	gotbutton = false;

	//Get a button from the input engine and store it at aUserData
	uint32_t* button = (uint32_t*)aUserData;
	button[0] = es_input->GetAnyButton(0);

	//Note wheather a button has beed pressed for next call
	gotbutton = (button[0] != 0xFFFFFFFF) ? true : false;
	return gotbutton;
}


