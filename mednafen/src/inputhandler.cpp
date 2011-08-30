#include <mednafen_includes.h>
#include "inputhandler.h"
#include "SettingGroupMenu.h"

class									InputEnumeratorBase : public InputEnumerator
{
	public:
										InputEnumeratorBase				() : GameInfo(0), PortInfo(0), DeviceInfo(0) {}
		virtual void					System							(const MDFNGI* aDescription) {GameInfo = aDescription;}
		virtual bool					Port							(const InputPortInfoStruct* aDescription) {PortInfo = aDescription; return true;}
		virtual bool					Device							(const InputDeviceInfoStruct* aDescription) {DeviceInfo = aDescription; return true;}

	protected:
		const MDFNGI*					GameInfo;
		const InputPortInfoStruct*		PortInfo;
		const InputDeviceInfoStruct*	DeviceInfo;

		char							StringBuffer[1024];
};

class									SettingGenerator : public InputEnumeratorBase
{
	public:
										SettingGenerator				(std::vector<MDFNSetting>& aSettings) : Settings(aSettings) {}

		virtual bool					Port							(const InputPortInfoStruct* aDescription)
		{
			InputEnumeratorBase::Port(aDescription);

			const char* defaultDeviceName = PortInfo->DefaultDevice ? PortInfo->DefaultDevice : PortInfo->DeviceInfo[0].ShortName;
			MDFNSetting thisinput = {Utility::VAPrintD("%s.esinput.port1", GameInfo->shortname), MDFNSF_NOFLAGS, "Input.", NULL, MDFNST_ENUM, defaultDeviceName, 0, 0, 0, 0, InputHandler::BuildPortEnum(*PortInfo)};
			Settings.push_back(thisinput);

			return true;
		}

		virtual void					Button							(const InputDeviceInputInfoStruct* aDescription)
		{
			if(aDescription->SettingName)
			{
				MDFNSetting	thisInput = {0, MDFNSF_CAT_INPUT, aDescription->Name, DeviceInfo->FullName, MDFNST_UINT, "0"};

				thisInput.name = Utility::VAPrintD("%s.esinput.%s.%s", GameInfo->shortname, DeviceInfo->ShortName, aDescription->SettingName);
				Settings.push_back(thisInput);

				//Rapid button support
				if(aDescription->Type == IDIT_BUTTON_CAN_RAPID)
				{
					thisInput.name = Utility::VAPrintD("%s.esinput.%s.%s_rapid", GameInfo->shortname, DeviceInfo->ShortName, aDescription->SettingName);
					Settings.push_back(thisInput);
				}
			}
		}

	private:
		std::vector<MDFNSetting>&		Settings;

};

class									ConfigurePrepper : public InputEnumeratorBase
{
	public:
		void							AddSetting						(const char* aName)
		{
			const std::multimap<uint32_t, MDFNCS>* settings = MDFNI_GetSettings();

			for(std::multimap<uint32, MDFNCS>::const_iterator iter = settings->begin(); iter != settings->end(); iter++)
			{
				if(strcmp(iter->second.name, aName) == 0)
				{
					Settings.push_back(&iter->second);
					return;
				}
			}
		}

		virtual bool					Port							(const InputPortInfoStruct* aDescription)
		{
			InputEnumeratorBase::Port(aDescription);

			const char* defaultDeviceName = PortInfo->DefaultDevice ? PortInfo->DefaultDevice : PortInfo->DeviceInfo[0].ShortName;
			AddSetting(Utility::VAPrint(StringBuffer, sizeof(StringBuffer), "%s.esinput.port1", GameInfo->shortname));

			return true;
		}

		virtual void					Button							(const InputDeviceInputInfoStruct* aDescription)
		{
			if(aDescription->SettingName)
			{
				AddSetting(Utility::VAPrint(StringBuffer, sizeof(StringBuffer), "%s.esinput.%s.%s", GameInfo->shortname, DeviceInfo->ShortName, aDescription->SettingName));

				//Rapid button support
				if(aDescription->Type == IDIT_BUTTON_CAN_RAPID)
				{
					AddSetting(Utility::VAPrint(StringBuffer, sizeof(StringBuffer), "%s.esinput.%s.%s_rapid", GameInfo->shortname, DeviceInfo->ShortName, aDescription->SettingName));
				}
			}
		}

		virtual void					Finish							()
		{
			SettingGroupMenu(Settings, "").Do();
		}

	private:
		std::vector<const MDFNCS*>		Settings;
};

class									SettingReader : public InputEnumeratorBase
{
	public:
										SettingReader					(InputHandler* aHandler, std::vector<InputHandler::InputInfo>& aInputs) : byte(0), bit(0), found(0), Handler(aHandler), Inputs(aInputs) {}

		virtual bool					Device							(const InputDeviceInfoStruct* aDescription)
		{
			InputEnumeratorBase::Device(aDescription);

			const char* type = MDFN_GetSettingS(Utility::VAPrint(StringBuffer, sizeof(StringBuffer), "%s.esinput.port1", GameInfo->shortname)).c_str();
			if(strcmp(type, DeviceInfo->ShortName) == 0)
			{
				MDFNI_SetInput(0, type, Handler->GetRawBits(0), 2);
				return true;
			}

			return false;
		}

		virtual void					Button							(const InputDeviceInputInfoStruct* aDescription)
		{
			InputHandler::InputInfo ii;
			memset(&ii, 0, sizeof(ii));

			//TODO: Support other types of inputs
			if(aDescription->Type == IDIT_BUTTON || aDescription->Type == IDIT_BUTTON_CAN_RAPID)
			{
				//Mednafen has the most convoluted input scheme. 
				ii.BitOffset = byte * 8 + bit;
				if(++bit == 8)
				{
					bit = 0;
					byte ++;
				}
			}

			ii.Button = MDFN_GetSettingUI(Utility::VAPrint(StringBuffer, sizeof(StringBuffer), "%s.esinput.%s.%s", GameInfo->shortname, DeviceInfo->ShortName, aDescription->SettingName));
			if(aDescription->Type == IDIT_BUTTON_CAN_RAPID)
			{
				ii.RapidButton = MDFN_GetSettingUI(Utility::VAPrint(StringBuffer, sizeof(StringBuffer), "%s.esinput.%s.%s_rapid", GameInfo->shortname, DeviceInfo->ShortName, aDescription->SettingName));
			}

			ii.Index = found ++;
			ii.Data = aDescription;
			Inputs.push_back(ii);
		}


	private:
		InputHandler*									Handler;
		std::vector<InputHandler::InputInfo>&			Inputs;


		std::vector<const MDFNCS*>		Settings;

		int								byte;
		int								bit;
		int								found;
};

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
}

void							InputHandler::EnumerateInputs			(const MDFNGI* aSystem, InputEnumerator* aEnumerator)
{
	assert(aSystem && aEnumerator);

	aEnumerator->System(aSystem);

	//Ports: TODO: Multiple ports
	if(aEnumerator->Port(&aSystem->InputInfo->Types[0]))
	{
		//Devices
		const InputDeviceInfoStruct* thisDevice = aSystem->InputInfo->Types[0].DeviceInfo;
		for(int i = 0; i != aSystem->InputInfo->Types[0].NumTypes; i ++, thisDevice ++)
		{
			if(aEnumerator->Device(thisDevice))
			{
				//Buttons
				const InputDeviceInputInfoStruct* thisButton = thisDevice->IDII;
				for(int j = 0; j != thisDevice->NumInputs; j ++, thisButton ++)
				{
					aEnumerator->Button(thisButton);
				}
			}
		}
	}

	aEnumerator->Finish();
}

void							InputHandler::Process					()
{
	//Update platform dependent input
	ESInput::Refresh();

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
			if((Inputs[i].Data->Type == IDIT_BUTTON || Inputs[i].Data->Type == IDIT_BUTTON_CAN_RAPID) && (ESInput::ButtonPressed(p, Inputs[i].Button)))
			{
				ControllerBits[p][byte] |= 1 << bit;
			}

			//Get any rapid press
			if((Inputs[i].Data->Type == IDIT_BUTTON_CAN_RAPID) && (ESInput::ButtonPressed(p, Inputs[i].RapidButton) && (RapidCount == 0)))
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
	ConfigurePrepper buttonList;
	EnumerateInputs(GameInfo, &buttonList);
	ReadSettings();
}

void							InputHandler::ReadSettings			()
{
	Inputs.clear();

	SettingReader reader(this, Inputs);
	EnumerateInputs(GameInfo, &reader);
}

void							InputHandler::GenerateSettings			(std::vector<MDFNSetting>& aSettings)
{
	SettingGenerator generator(aSettings);

	for(int i = 0; i != MDFNSystems.size(); i ++)
	{
		EnumerateInputs(MDFNSystems[i], &generator);
	}
}

MDFNSetting_EnumList*			InputHandler::BuildPortEnum				(const InputPortInfoStruct& aPort)
{
	MDFNSetting_EnumList* results = new MDFNSetting_EnumList[aPort.NumTypes + 1];

	for(int i = 0; i != aPort.NumTypes; i ++)
	{
		results[i].string = aPort.DeviceInfo[i].ShortName;
		results[i].number = i;
		results[i].description = aPort.DeviceInfo[i].FullName;
		results[i].description_extra = "";
	}

	memset(&results[aPort.NumTypes], 0, sizeof(MDFNSetting_EnumList));

	return results;
}

int								InputHandler::GetButton					(void* aUserData, Summerface* aInterface, const std::string& aWindow, uint32_t aButton)
{
	//Conduit for config interface
	static bool gotbutton = true;

	//Don't continue until all buttons are released
	if(gotbutton && ESInput::GetAnyButton(0) != 0xFFFFFFFF)
	{
		return 0;
	}

	//Note that no buttons are pressed
	gotbutton = false;

	//Get a button from the input engine and store it at aUserData
	uint32_t* button = (uint32_t*)aUserData;
	button[0] = ESInput::GetAnyButton(0);

	//Note wheather a button has beed pressed for next call
	gotbutton = (button[0] != 0xFFFFFFFF) ? true : false;
	return gotbutton ? -1 : 0;
}


