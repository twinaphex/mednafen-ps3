#include <mednafen_includes.h>
#include "SettingCategoryMenu.h"
#include "SettingGroupMenu.h"


								SettingCategoryMenu::SettingCategoryMenu		(const std::string& aDefaultCategory) :
	List(Area(10, 10, 80, 80)),
	Interface("Categories", &List, false)
{
	//Cache the setting values from mednafen
	LoadSettings();

	//Setup the category list
	List.SetHeader("Choose Setting Category");

	//Stuff the category list
	for(SettingCollection::iterator i = Settings.begin(); i != Settings.end(); i ++)
	{
		List.AddItem(new ListItem(TranslateCategory(i->first.c_str()), "", i->first));
	}

	//Sort the list and choose the default selection
	List.Sort();
	List.SetSelection(TranslateCategory(aDefaultCategory.c_str()));
}

void							SettingCategoryMenu::Do							()
{
	while(!WantToDie())
	{
		//Run the category list
		Interface.Do();

		//Leave if the category list is canceled and everything checks out
		if(!List.WasCanceled())
		{
			SettingGroupMenu(Settings[List.GetSelected()->UserData], List.GetSelected()->UserData).Do();
		}
		else
		{
			//Done
			return;
		}
	}
}

void							SettingCategoryMenu::LoadSettings				()
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


std::string						SettingCategoryMenu::TranslateCategory			(const char* aCategoryName)
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

