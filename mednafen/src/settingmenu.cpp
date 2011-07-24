#include <mednafen_includes.h>
#include "mednafen_help.h"
#include "settingmenu.h"

static bool						CompareItems									(smartptr::shared_ptr<SettingItem> a, smartptr::shared_ptr<SettingItem> b)
{
	//
	if(a->GetGroup() != b->GetGroup())
	{
		if(a->GetGroup() == "MODULE")												return true;
		if(b->GetGroup() == "MODULE")												return false;

		if(a->GetGroup() == "SYSTEM")												return true;
		if(b->GetGroup() == "SYSTEM")												return false;

		if(a->GetGroup() == "DISPLAY")												return true;
		if(b->GetGroup() == "DISPLAY")												return false;

		return a->GetGroup() < b->GetGroup();
	}

	//Keep enable at the top
	if(a->GetText().find(".enable") != std::string::npos)												return true;
	if(b->GetText().find(".enable") != std::string::npos)												return false;

	//Undertune items at the bottom
	if(a->GetText().find("undertune") != std::string::npos)												return false;
	if(b->GetText().find("undertune") != std::string::npos)												return true;

	//Tblus items at the bottom
	if(a->GetText().find("tblur") != std::string::npos)													return false;
	if(b->GetText().find("tblur") != std::string::npos)													return true;

	//Force mono items at the bottom
	if(a->GetText().find("forcemono") != std::string::npos)												return false;
	if(b->GetText().find("forcemono") != std::string::npos)												return true;

	//Keep es system settings above others
	if(a->GetText().find(".es.") != std::string::npos && b->GetText().find(".es.") != std::string::npos)	return a->GetText() < b->GetText();
	if(a->GetText().find(".es.") != std::string::npos)													return false;
	if(b->GetText().find(".es.") != std::string::npos)													return true;

	//Standard items at the bottom
	return a->GetText() < b->GetText();
}

								SettingMenu::SettingMenu						(const std::string& aDefaultCategory) :
	List(smartptr::make_shared<SettingListType>(Area(10, 10, 80, 80))),
	CategoryList(smartptr::make_shared<CategoryListType>(Area(10, 10, 80, 80))),
	CategoryInterface(Summerface::Create("Categories", CategoryList))
{
	//Cache the setting values from mednafen
	LoadSettings();

	//Setup the category list
	CategoryList->SetHeader("Choose Setting Category");

	//Stuff the category list
	for(SettingCollection::iterator i = Settings.begin(); i != Settings.end(); i ++)
	{
		CategoryList->AddItem(smartptr::make_shared<CategoryListItem>(TranslateCategory(i->first.c_str()), "", i->first));
	}

	//Sort the list and choose the default selection
	CategoryList->Sort();
	CategoryList->SetSelection(TranslateCategory(aDefaultCategory.c_str()));
}

void							SettingMenu::Do									()
{
	while(!WantToDie())
	{
		//Run the category list
		CategoryInterface->Do();

		//Leave if the category list is canceled and everything checks out
		if(!CategoryList->WasCanceled())
		{
			//Clear the setting list
			List->ClearItems();

			//Add all settings from the catagory
			const std::vector<const MDFNCS*>& items = Settings[CategoryList->GetSelected()->UserData];
			for(int i = 0; i != items.size(); i ++)
			{
				List->AddItem(smartptr::make_shared<SettingItem>(items[i], TranslateGroup(*items[i], CategoryList->GetSelected()->UserData)));
			}

			//Sort the setting list
			List->Sort(CompareItems);

			//HACK: Create the interface without input wait until the header is updated
			Summerface_Ptr sface = Summerface::Create("SettingList", List);
			sface->SetInputWait(false);
			sface->AttachConduit(smartptr::make_shared<SummerfaceTemplateConduit<SettingMenu> >(this));
			sface->Do();
		}
		else
		{
			//Done
			return;
		}
	}
}

int								SettingMenu::HandleInput						(Summerface_Ptr aInterface, const std::string& aWindow, uint32_t aButton)
{
	assert(List && List->GetItemCount() != 0);

	//Get a refence to the setting
	const MDFNCS& Setting = *(const MDFNCS*)List->GetSelected()->Setting;

	//HACK: Set the input wait flag if no header refresh is waiting
	if(RefreshHeader == false)
	{
		aInterface->SetInputWait(true);
	}

	//Refresh the header
	DoHeaderRefresh();

	//Check for and handle setting types
	if(Setting.desc->type == MDFNST_BOOL && HandleBool(aButton, Setting))
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
		std::vector<std::string> nomarks;
		FileSelect browse("Select File", nomarks, "");
		
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

void							SettingMenu::DoHeaderRefresh					()
{
	//If a refresh is scheduled and a list item is available
	if(RefreshHeader)
	{
		//Set the header
		List->SetHeader(List->GetSelected()->Setting->desc->description);
	}
}

bool							SettingMenu::HandleBool							(uint32_t aButton, const MDFNCS& aSetting)
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

bool							SettingMenu::HandleInt							(uint32_t aButton, const MDFNCS& aSetting)
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

bool							SettingMenu::HandleEnum							(uint32_t aButton, const MDFNCS& aSetting)
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
		EnumListType_Ptr list = smartptr::make_shared<EnumListType>(Area(10, 10, 80, 80));
		list->SetHeader(std::string("Choose ") + aSetting.name + "'s new value:");

		//Place all settings into the list
		while(values && values->string)
		{
			list->AddItem(smartptr::make_shared<SummerfaceItem>(values->string, ""));
			values ++;
		}

		assert(list->GetItemCount() != 0);

		//Run the list
		Summerface::Create("List", list)->Do();

		//Get the new setting and feed it to mednafen
		if(!list->WasCanceled())
		{
			MDFNI_SetSetting(aSetting.name, list->GetSelected()->GetText().c_str());
			MednafenEmu::ReadSettings();
		}

		//Done
		return true;
	}

	//Block using the file browser to set an enum value
	return aButton == ES_BUTTON_TAB;
}


void							SettingMenu::LoadSettings						()
{
	//Get the settings map from mednafen
	const std::multimap<uint32_t, MDFNCS>* settings = MDFNI_GetSettings();
	assert(settings);

	//Iterate it and get all of the settings
	for(std::multimap<uint32, MDFNCS>::const_iterator iter = settings->begin(); iter != settings->end(); iter++)
	{
		//Default category is "General Settings"
		std::string header = "general";
	
		//HACK: Don't use filesys.*samedir
		if(strstr(iter->second.name, "filesys.") != 0 && strstr(iter->second.name, "samedir") != 0)
		{
			MDFNI_SetSettingB(iter->second.name, false);
			continue;
		}

		//Don't list input or the bookmarks settings, these are set in a different manner	
		if(std::string(iter->second.name).find(".esinput.") != std::string::npos || std::string(iter->second.name).find(".bookmarks") != std::string::npos)
		{
			continue;
		}
	
		//Get the header, all text leading up to the first dot
		if(std::string(iter->second.name).find(".") != std::string::npos)
		{
			header = std::string(iter->second.name).substr(0, std::string(iter->second.name).find_first_of("."));
		}

		//Add the setting into the local cache
		Settings[header].push_back(&iter->second);
	}
}

std::string						SettingMenu::TranslateGroup						(const MDFNCS& aSetting, const std::string& aSystem)
{
	std::string settingName = aSetting.name;

	if(!aSystem.empty())
	{
		if(settingName.find(".tblur") != std::string::npos)				return "TBLUR";
		if(settingName.find(".undertune") != std::string::npos)			return "DISPLAY";
		if(settingName.find(".underscanadjust") != std::string::npos)	return "DISPLAY";
		if(settingName.find(".display") != std::string::npos)			return "DISPLAY";
		if(settingName.find(".aspect") != std::string::npos)			return "DISPLAY";
		if(settingName.find(".shader") != std::string::npos)			return "DISPLAY";
		if(settingName.find(".enable") != std::string::npos)			return "MODULE";
		if(settingName.find(".rewind") != std::string::npos)			return "MODULE";
		if(settingName.find(".forcemono") != std::string::npos)			return "MODULE";
		if(settingName.find(".autosave") != std::string::npos)			return "MODULE";
		return "SYSTEM";
	}
	else
	{
		return "";
	}
}

std::string						SettingMenu::TranslateCategory					(const char* aCategoryName)
{
	//The name of any emulator module is the module's full name
	for(int i = 0; i != MDFNSystems.size(); i ++)
	{
		if(strcmp(aCategoryName, MDFNSystems[i]->shortname) == 0)
		{
			return MDFNSystems[i]->fullname;
		}
	}

	//Other generic setting categories
	//NOTE: A space is added to ensure they bubble to the top of sorting
	const char* settingscats[] = 
	{
		"qtrecord", " Video Recording",
		"net", " Netplay",
		"general", " General Settings",
		"ftp", " FTP Client",
		"filesys", " File System",
		"cdrom", " CDROM Settings"
	};

	for(int i = 0; i != 6; i ++)
	{
		if(strcmp(settingscats[i * 2], aCategoryName) == 0)
		{
			return settingscats[i * 2 + 1];
		}
	}

	//No translation, use the default
	return aCategoryName;
}

