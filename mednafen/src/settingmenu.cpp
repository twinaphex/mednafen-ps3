#include <mednafen_includes.h>
#include "mednafen_help.h"
#include "settingmenu.h"

static bool						CompareItems									(SummerfaceItem_Ptr a, SummerfaceItem_Ptr b)
{
	//Keep enable at the top
	if(a->GetText().find(".enable") != std::string::npos)												return true;
	if(b->GetText().find(".enable") != std::string::npos)												return false;

	//Undertune items at the bottom
	if(a->GetText().find("undertune") != std::string::npos)												return false;
	if(b->GetText().find("undertune") != std::string::npos)												return true;

	//Keep es system settings above others
	if(a->GetText().find(".es.") != std::string::npos && b->GetText().find(".es.") != std::string::npos)	return a->GetText() < b->GetText();
	if(a->GetText().find(".es.") != std::string::npos)													return true;
	if(b->GetText().find(".es.") != std::string::npos)													return false;

	//Standard items at the bottom
	return a->GetText() < b->GetText();
}

bool							SettingLineView::Input							(uint32_t aButton)
{
	if(!WeakList.expired())
	{
		//Get a pointer to the list
		SummerfaceList_Ptr List = WeakList.lock();

		//Leave if there is no list item
		if(!List->GetSelected() || !List->GetSelected()->IntProperties["MDFNCS"] || !List->GetInterface())
		{
			return false;
		}

		//Get a refence to the setting
		const MDFNCS& Setting = *(const MDFNCS*)List->GetSelected()->IntProperties["MDFNCS"];

		//HACK: Set the input wait flag if no header refresh is waiting
		if(RefreshHeader == false && List->GetInterface())
		{
			List->GetInterface()->SetInputWait(true);
		}

		//Refresh the header
		DoHeaderRefresh(List);

		//Check for and handle setting types
		if(Setting.desc->type == MDFNST_BOOL && HandleBool(aButton, Setting))
		{
			return false;
		}
		else if((Setting.desc->type == MDFNST_UINT || Setting.desc->type == MDFNST_INT) && HandleInt(aButton, Setting))
		{
			return false;
		}
		else if(Setting.desc->type == MDFNST_ENUM && HandleEnum(aButton, Setting))
		{
			return false;
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
			return false;
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

			return false;
		}

		//If the button isn't left or right pass it to the AnchoredListView class.
		if(aButton != ES_BUTTON_LEFT && aButton != ES_BUTTON_RIGHT)
		{
			//Cache the selected item
			SummerfaceItem_Ptr selected = List->GetSelected();

			//Pass it to the base
			if(!AnchoredListView::Input(aButton))
			{
				//Schedule a header update if needed
				if(selected != List->GetSelected())
				{
					RefreshHeader = true;
					DoHeaderRefresh(List);
				}

				return false;
			}
			//The base list view said to exit
			else
			{
				return true;
			}
		}
	}

	//Default return value
	return false;
}

void							SettingLineView::DoHeaderRefresh				(SummerfaceList_Ptr aList)
{
	//If a refresh is scheduled and a list item is available
	if(RefreshHeader && aList && aList->GetSelected() && aList->GetSelected()->IntProperties["MDFNCS"])
	{
		//Set the header
		const MDFNCS& Setting = *(const MDFNCS*)aList->GetSelected()->IntProperties["MDFNCS"];
		aList->SetHeader(Setting.desc->description);
	}
}

bool							SettingLineView::DrawItem						(SummerfaceList_Ptr aList, SummerfaceItem_Ptr aItem, uint32_t aX, uint32_t aY, bool aSelected)
{
	//Tell the base class to do its part
	AnchoredListView::DrawItem(aList, aItem, aX, aY, aSelected);

	//Get a pointer to the setting and, if valid, draw it
	const MDFNCS* setting = (const MDFNCS*)aItem->IntProperties["MDFNCS"];
	if(setting)
	{
		//Draw using special casing for bools
		aList->GetFont()->PutString((setting->desc->type == MDFNST_BOOL) ? (MDFN_GetSettingB(setting->name) ? "ON" : "OFF") : MDFN_GetSettingS(setting->name).c_str(), aX + (ESVideo::GetClip().Width / 3) * 2, aY, aSelected ? aItem->GetHighLightColor() : aItem->GetNormalColor(), true);
	}

	return false;
}

bool							SettingLineView::HandleBool						(uint32_t aButton, const MDFNCS& aSetting)
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

bool							SettingLineView::HandleInt						(uint32_t aButton, const MDFNCS& aSetting)
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

bool							SettingLineView::HandleEnum						(uint32_t aButton, const MDFNCS& aSetting)
{
	//Get a poitner to the enumeration values
	const MDFNSetting_EnumList* values = aSetting.desc->enum_list;

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
		SummerfaceList_Ptr list = smartptr::make_shared<SummerfaceList>(Area(10, 10, 80, 80));
		list->SetView(smartptr::make_shared<AnchoredListView>(list));
		list->SetHeader(std::string("Choose ") + aSetting.name + "'s new value:");

		//Place all settings into the list
		while(values && values->string)
		{
			list->AddItem(smartptr::make_shared<SummerfaceItem>(values->string, ""));
			values ++;
		}

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

	//Block using the file browser to set an int value
	return aButton == ES_BUTTON_TAB;
}



								SettingMenu::SettingMenu						(const std::string& aDefaultCategory) :
	List(smartptr::make_shared<SummerfaceList>(Area(10, 10, 80, 80))),
	CategoryList(smartptr::make_shared<SummerfaceList>(Area(10, 10, 80, 80))),
	Interface(Summerface::Create("Categories", CategoryList))
{
	//Cache the setting values from mednafen
	LoadSettings();

	//Setup the category list
	CategoryList->SetView(smartptr::make_shared<AnchoredListView>(CategoryList));
	CategoryList->SetHeader("Choose Setting Category");

	//Setup the setting list
	List->SetView(smartptr::make_shared<SettingLineView>(List));

	//Stuff the category list
	for(SettingCollection::iterator i = Settings.begin(); i != Settings.end(); i ++)
	{
		//Create the item
		SummerfaceItem_Ptr item = smartptr::make_shared<SummerfaceItem>(TranslateCategory(i->first.c_str()), "");
		item->Properties["CATEGORY"] = i->first;

		//Highlight it if it is the default category
		if(item->Properties["CATEGORY"] == aDefaultCategory)
		{
			item->SetColors(Colors::SpecialNormal, Colors::SpecialHighLight);
		}

		//Stash it in the list
		CategoryList->AddItem(item);
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
		Interface->Do();

		//Leave if the category list is canceld and everything checks out
		if(!CategoryList->WasCanceled() && CategoryList->GetSelected() && !CategoryList->GetSelected()->Properties["CATEGORY"].empty())
		{
			//Clear the setting list
			List->ClearItems();

			//Add all settings from the catagory
			std::vector<const MDFNCS*> items = Settings[CategoryList->GetSelected()->Properties["CATEGORY"]];
			for(int i = 0; i != items.size(); i ++)
			{
				SummerfaceItem_Ptr item = smartptr::make_shared<SummerfaceItem>(items[i]->name, "");
				item->IntProperties["MDFNCS"] = (uint64_t)items[i];
				List->AddItem(item);
			}

			//Sort the setting list
			List->Sort(CompareItems);

			//HACK: Create the interface without input wait until the header is update
			Summerface_Ptr sface = Summerface::Create("SettingList", List);
			sface->SetInputWait(false);
			sface->Do();
		}
		else
		{
			//Done
			return;
		}
	}
}

void							SettingMenu::LoadSettings						()
{
	//Get the settings map from mednafen
	const std::multimap<uint32_t, MDFNCS>* settings = MDFNI_GetSettings();

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

