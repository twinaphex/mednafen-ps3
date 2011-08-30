#include <mednafen_includes.h>
#include "inputhandler.h"
#include "SettingGroupMenu.h"

namespace
{
	struct										InputInfo
	{
		uint32_t								BitOffset;
		uint32_t								Button;
		uint32_t								RapidButton;
		const InputDeviceInputInfoStruct*		Data;
	};

	const MDFNGI*								GameInfo;
	uint32_t									RapidCount;
	std::vector<InputInfo>						Inputs;
	uint8_t										ControllerBits[16][256];
}


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

//Generate settings for each button to pass to the mednafen core
class									SettingGenerator : public InputEnumeratorBase
{
	public:
										SettingGenerator				(std::vector<MDFNSetting>& aSettings) : Settings(aSettings) {}

		virtual bool					Port							(const InputPortInfoStruct* aDescription)
		{
			InputEnumeratorBase::Port(aDescription);

			//Build a list of devices (Never Deleted)
			MDFNSetting_EnumList* devices = new MDFNSetting_EnumList[PortInfo->NumTypes + 1];
			MDFNSetting_EnumList* thisDevice = devices;
			InputDeviceInfoStruct* thatDevice = PortInfo->DeviceInfo;

			for(int i = 0; i != PortInfo->NumTypes; i ++, thisDevice ++, thatDevice ++)
			{
				thisDevice->string = thatDevice->ShortName;
				thisDevice->number = i;
				thisDevice->description = thatDevice->FullName;
				thisDevice->description_extra = "";
			}

			//Terminate device enumeration
			memset(thisDevice, 0, sizeof(MDFNSetting_EnumList));

			//Get the default plugged device
			const char* defaultDeviceName = PortInfo->DefaultDevice ? PortInfo->DefaultDevice : PortInfo->DeviceInfo[0].ShortName;

			//Stash the setting
			MDFNSetting thisinput = {Utility::VAPrintD("%s.esinput.port1", GameInfo->shortname), MDFNSF_NOFLAGS, "Input.", NULL, MDFNST_ENUM, defaultDeviceName, 0, 0, 0, 0, devices};
			Settings.push_back(thisinput);

			return true;
		}

		virtual void					Button							(const InputDeviceInputInfoStruct* aDescription)
		{
			//Note that VAPrintD generates strings on the heap that will not be passed to free
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

//Configure the input buttons for a given system
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

//Read the settings for processing for the given system
class									SettingReader : public InputEnumeratorBase
{
	public:
										SettingReader					() : bit(0) {}

		virtual bool					Device							(const InputDeviceInfoStruct* aDescription)
		{
			InputEnumeratorBase::Device(aDescription);

			const char* type = MDFN_GetSettingS(Utility::VAPrint(StringBuffer, sizeof(StringBuffer), "%s.esinput.port1", GameInfo->shortname)).c_str();
			if(strcmp(type, DeviceInfo->ShortName) == 0)
			{
				MDFNI_SetInput(0, type, &ControllerBits[0], 2);
				return true;
			}

			return false;
		}

		virtual void					Button							(const InputDeviceInputInfoStruct* aDescription)
		{
			if(aDescription->Type == IDIT_BUTTON || aDescription->Type == IDIT_BUTTON_CAN_RAPID)
			{
				InputInfo ii;
				memset(&ii, 0, sizeof(ii));

				ii.BitOffset = bit ++;
				ii.Button = MDFN_GetSettingUI(Utility::VAPrint(StringBuffer, sizeof(StringBuffer), "%s.esinput.%s.%s", GameInfo->shortname, DeviceInfo->ShortName, aDescription->SettingName));
				if(aDescription->Type == IDIT_BUTTON_CAN_RAPID)
				{
					ii.RapidButton = MDFN_GetSettingUI(Utility::VAPrint(StringBuffer, sizeof(StringBuffer), "%s.esinput.%s.%s_rapid", GameInfo->shortname, DeviceInfo->ShortName, aDescription->SettingName));
				}

				ii.Data = aDescription;
				Inputs.push_back(ii);
			}
		}


	private:
		InputHandler*					Handler;

		std::vector<const MDFNCS*>		Settings;

		int								bit;
};

void							InputHandler::SetGameInfo						(const MDFNGI* aSystem)
{
	GameInfo = aSystem;

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

void							InputHandler::EnumerateInputs					(const MDFNGI* aSystem, InputEnumerator* aEnumerator)
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

void							InputHandler::Process							()
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


void							InputHandler::Configure							()
{
	ConfigurePrepper buttonList;
	EnumerateInputs(GameInfo, &buttonList);
	ReadSettings();
}

void							InputHandler::ReadSettings						()
{
	Inputs.clear();

	SettingReader reader;
	EnumerateInputs(GameInfo, &reader);
}

void							InputHandler::GenerateSettings					(std::vector<MDFNSetting>& aSettings)
{
	SettingGenerator generator(aSettings);

	for(int i = 0; i != MDFNSystems.size(); i ++)
	{
		EnumerateInputs(MDFNSystems[i], &generator);
	}
}


