#include <mednafen_includes.h>
#include "mednafen_help.h"
#include "SettingGroupMenu.h"
#include "inputhandler.h"

#include "src/utility/Files/FileSelect.h"

std::string						SettingItem::GetText							()
{
	if(Setting->desc->flags & MDFNSF_CAT_INPUT)
	{
		return std::string(Setting->name) +  "\t" + ESInput::ButtonName(0, MDFN_GetSettingUI(Setting->name));
	}
	else
	{
		return std::string(Setting->name) + "\t" + ((Setting->desc->type == MDFNST_BOOL) ? (MDFN_GetSettingB(Setting->name) ? "ON" : "OFF") : MDFN_GetSettingS(Setting->name));
	}
}


								SettingGroupMenu::SettingGroupMenu				(SettingGroup& aSettings, const std::string& aSystemName) :
	List(Area(10, 10, 80, 80)),
	Interface("Settings", &List, false),
	RefreshHeader(true)
{
	//Add all settings from the catagory
	for(SettingGroup::const_iterator i = aSettings.begin(); i != aSettings.end(); i ++)
	{
		List.AddItem(*i);		
	}

	aSettings.clear();

	//Setup interface
	Interface.SetInputWait(false);
	Interface.AttachConduit(new SummerfaceTemplateConduit<SettingGroupMenu>(this));
}

void							SettingGroupMenu::Do							()
{
	Interface.Do();
}

int								SettingGroupMenu::HandleInput					(Summerface* aInterface, const std::string& aWindow, uint32_t aButton)
{
	assert(List.GetItemCount() != 0);

	//Get a refence to the setting
	const MDFNCS& Setting = *(const MDFNCS*)List.GetSelected()->Setting;

	//HACK: Set the input wait flag if no header refresh is waiting
	if(RefreshHeader == false)
	{
		aInterface->SetInputWait(true);
	}

	//Refresh the header
	DoHeaderRefresh();

	//Check for and handle setting types
	if(Setting.desc->flags & MDFNSF_CAT_INPUT && HandleButton(aButton, Setting))
	{
		return 1;
	}
	else if(Setting.desc->type == MDFNST_BOOL && HandleBool(aButton, Setting))
	{
		return 1;
	}
	else if((Setting.desc->type == MDFNST_UINT || Setting.desc->type == MDFNST_INT) && HandleInt(aButton, Setting))
	{
		return 1;
	}
	else if(Setting.desc->type == MDFNST_ENUM && HandleEnum(aButton, Setting))
	{
		return 1;
	}

	//Fire up the keyboard (may be canceled by setting handlers)
	if(aButton == ES_BUTTON_ACCEPT)
	{
		//TODO: Error Checking?
		std::string result = ESSUB_GetString(Setting.name, MDFN_GetSettingS(Setting.name));
		MDFNI_SetSetting(Setting.name, result.c_str());
		MednafenEmu::ReadSettings();
		return false;
	}
	//Set setting back to its default
	else if(aButton == ES_BUTTON_SHIFT)
	{
		MDFNI_SetSetting(Setting.name, Setting.desc->default_value);
		MednafenEmu::ReadSettings();
		return 1;
	}
	//Browse for a file and use its path as the setting value (may be canceled by setting handlers)
	else if(aButton == ES_BUTTON_TAB)
	{
		FileSelect browse(_("Select File"), "", "");
		
		std::string result = browse.GetFile();
		if(result.length() != 0)
		{
			MDFNI_SetSetting(Setting.name, result.c_str());
			MednafenEmu::ReadSettings();
		}

		return 1;
	}

	//Eat any left and right buttons, to prevent paging the list
	return (aButton == ES_BUTTON_LEFT || aButton == ES_BUTTON_RIGHT) ? 1 : 0;
}

void							SettingGroupMenu::DoHeaderRefresh				()
{
	//If a refresh is scheduled and a list item is available
	if(RefreshHeader)
	{
		//Set the header
		List.SetHeader(List.GetSelected()->Setting->desc->description);
	}
}

bool							SettingGroupMenu::HandleButton					(uint32_t aButton, const MDFNCS& aSetting)
{
	if(aButton == ES_BUTTON_LEFT || aButton == ES_BUTTON_RIGHT || aButton == ES_BUTTON_ACCEPT)
	{
		SummerfaceButton buttonGetter(Area(10, 30, 80, 10), aSetting.desc->description);
		Summerface sface("InputWindow", &buttonGetter, false);
		sface.SetInputWait(false);
		sface.Do();
		MDFNI_SetSettingUI(aSetting.name, buttonGetter.GetButton());

		MednafenEmu::ReadSettings();

		return true;
	}

	//Block using the file browser and keyboard to set an input value
	return aButton == ES_BUTTON_TAB || aButton == ES_BUTTON_ACCEPT;
}

bool							SettingGroupMenu::HandleBool					(uint32_t aButton, const MDFNCS& aSetting)
{
	//Toggle if the button is left, right, or accept
	if(aButton == ES_BUTTON_LEFT || aButton == ES_BUTTON_RIGHT || aButton == ES_BUTTON_ACCEPT)
	{
		MDFNI_SetSettingB(aSetting.name, MDFN_GetSettingB(aSetting.name) == 0);
		MednafenEmu::ReadSettings();
		return true;
	}

	//Block using the file browser and keyboard to set a bool value
	return aButton == ES_BUTTON_TAB || aButton == ES_BUTTON_ACCEPT;
}

bool							SettingGroupMenu::HandleInt						(uint32_t aButton, const MDFNCS& aSetting)
{
	//Get the change to be applied
	int32_t value = (aButton == ES_BUTTON_LEFT) ? -1 : 0;
	value = (aButton == ES_BUTTON_RIGHT) ? 1 : value;

	//Handle any changes
	if(value != 0)
	{
		//Handle differences between UINT and INT settings
		if(aSetting.desc->type == MDFNST_UINT)
		{
			MDFNI_SetSettingUI(aSetting.name, MDFN_GetSettingUI(aSetting.name) + value);
		}
		else
		{
			char buffer[256];
			snprintf(buffer, 256, "%lli", (long long int)MDFN_GetSettingI(aSetting.name) + value);
			MDFNI_SetSetting(aSetting.name, buffer);
		}

		//Tell the emulator to refresh then leave
		MednafenEmu::ReadSettings();
		return true;
	}

	//Block using the file browser to set an int value
	return aButton == ES_BUTTON_TAB;
}

bool							SettingGroupMenu::HandleEnum					(uint32_t aButton, const MDFNCS& aSetting)
{
	//Get a poitner to the enumeration values
	const MDFNSetting_EnumList* values = aSetting.desc->enum_list;
	assert(values);

	//Cycle values with left and right
	if(aButton == ES_BUTTON_LEFT || aButton == ES_BUTTON_RIGHT)
	{
		//Get the direction to move
		int32_t value = (aButton == ES_BUTTON_LEFT) ? -1 : 0;
		value = (aButton == ES_BUTTON_RIGHT) ? 1 : value;

		//Get the settings current value
		std::string oldvalue = MDFN_GetSettingS(aSetting.name);

		//Advance the values pointer to the selected item
		while(values && values->string && strcmp(values->string, oldvalue.c_str()))
		{
			values ++;
		}

		//Block moving before the first value
		if(values && values->string && !(values == aSetting.desc->enum_list && value < 0))
		{
			//Update the values
			values += value;

			//Block moving past the last value
			if(values->string)
			{
				//Update and leave
				MDFNI_SetSetting(aSetting.name, values->string);
				MednafenEmu::ReadSettings();

				return true;
			}
		}
	}
	//Choose from list with accept
	else if(aButton == ES_BUTTON_ACCEPT)
	{
		//Create the list
		EnumListType list(Area(10, 10, 80, 80));
		list.SetHeader(_("Choose %s's new value:"), aSetting.name);

		//Place all settings into the list
		while(values && values->string)
		{
			list.AddItem(new SummerfaceItem(values->string, ""));
			values ++;
		}

		assert(list.GetItemCount() != 0);

		//Run the list
		Summerface("List", &list, false).Do();

		//Get the new setting and feed it to mednafen
		if(!list.WasCanceled())
		{
			MDFNI_SetSetting(aSetting.name, list.GetSelected()->GetText().c_str());
			MednafenEmu::ReadSettings();
		}

		//Done
		return true;
	}

	//Block using the file browser to set an enum value
	return aButton == ES_BUTTON_TAB;
}


