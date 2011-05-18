#include <mednafen_includes.h>

static bool						CompareItems									(SummerfaceItem_Ptr a, SummerfaceItem_Ptr b)
{
	//Keep enable at the top
	if(a->GetText().find(".enable") != std::string::npos)												return true;
	if(b->GetText().find(".enable") != std::string::npos)												return false;

	//Keep ps3 system settings above others
	if(a->GetText().find(".es.") != std::string::npos && b->GetText().find(".es.") != std::string::npos)	return a->GetText() < b->GetText();
	if(a->GetText().find(".es.") != std::string::npos)													return true;
	if(b->GetText().find(".es.") != std::string::npos)													return false;

	//Standard items at the bottom
	return a->GetText() < b->GetText();
}

void							SettingLineView::DoHeaderRefresh				()
{
	if(RefreshHeader)
	{
		RefreshHeader = false;

		SummerfaceList_Ptr List = WeakList.lock();

		const MDFNCS& Setting = *(const MDFNCS*)List->GetSelected()->IntProperties["MDFNCS"];
		List->SetHeader(Setting.desc->description);
	}
}

bool							SettingLineView::DrawItem						(SummerfaceItem_Ptr aItem, uint32_t aX, uint32_t aY, bool aSelected)
{
	AnchoredListView::DrawItem(aItem, aX, aY, aSelected);

	const MDFNCS* setting = (const MDFNCS*)aItem->IntProperties["MDFNCS"];

	if(setting)
	{
		char buffer[256];
		snprintf(buffer, 250, "%s", (setting->desc->type == MDFNST_BOOL) ? (MDFN_GetSettingB(setting->name) ? "ON" : "OFF") : MDFN_GetSettingS(setting->name).c_str());

//HACK: Use real font
	  //  List->LabelFont->PutString(buffer, aX + (es_video->GetClip().Width / 3) * 2, aY, aSelected ? aItem->GetHighLightColor() : aItem->GetNormalColor());
		FontManager::GetBigFont()->PutString(buffer, aX + (es_video->GetClip().Width / 3) * 2, aY, aSelected ? aItem->GetHighLightColor() : aItem->GetNormalColor());

	}
	return false;
}

bool							SettingLineView::HandleBool						(const MDFNCS& aSetting)
{
	if(es_input->ButtonDown(0, ES_BUTTON_LEFT) || es_input->ButtonDown(0, ES_BUTTON_RIGHT))
	{
		MDFNI_SetSettingB(aSetting.name, MDFN_GetSettingB(aSetting.name) == 0);
		MednafenEmu::ReadSettings();
		return true;
	}

	return false;
}

bool							SettingLineView::HandleEnum						(const MDFNCS& aSetting)
{
	int32_t value = es_input->ButtonDown(0, ES_BUTTON_LEFT) ? -1 : 0;
	value = es_input->ButtonDown(0, ES_BUTTON_RIGHT) ? 1 : value;

	if(value != 0)
	{
		std::string oldvalue = MDFN_GetSettingS(aSetting.name);
		uint32_t oldvaluei = 0;
				
		while(1)
		{
			if(aSetting.desc->enum_list[oldvaluei].string == 0)
			{
				return true;
			}

			if(strcasecmp(aSetting.desc->enum_list[oldvaluei].string, oldvalue.c_str()) == 0)
			{
				break;
			}
						
			oldvaluei ++;
		}
		
		int32_t newvalue = oldvaluei + value;
				
		if(newvalue < 0)
		{
			return true;
		}
				
		for(int i = 0; i != newvalue + 1; i ++)
		{
			if(aSetting.desc->enum_list[i].string == 0)
			{
				return true;
			}
		}
				
		MDFNI_SetSetting(aSetting.name, aSetting.desc->enum_list[newvalue].string);
		MednafenEmu::ReadSettings();
		return true;
	}

	return false;
}

bool							SettingLineView::HandleInt						(const MDFNCS& aSetting)
{
	int32_t value = es_input->ButtonPressed(0, ES_BUTTON_LEFT) ? -1 : 0;
	value = es_input->ButtonPressed(0, ES_BUTTON_RIGHT) ? 1 : value;

	if(value != 0)
	{
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

		MednafenEmu::ReadSettings();
		return true;
	}

	return false;
}

bool							SettingLineView::Input							()
{
//TODO: Error check
	SummerfaceList_Ptr List = WeakList.lock();

	const MDFNCS& Setting = *(const MDFNCS*)List->GetSelected()->IntProperties["MDFNCS"];

	DoHeaderRefresh();

	if(Setting.desc->type == MDFNST_BOOL && HandleBool(Setting))
	{
		return false;
	}
	else if((Setting.desc->type == MDFNST_UINT || Setting.desc->type == MDFNST_INT) && HandleInt(Setting))
	{
		return false;
	}
	else if(Setting.desc->type == MDFNST_ENUM && HandleEnum(Setting))
	{
		return false;
	}

	if(es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
	{
		std::string result = ESSUB_GetString(Setting.name, MDFN_GetSettingS(Setting.name));
		MDFNI_SetSetting(Setting.name, result.c_str());
		return false;
	}
	else if(es_input->ButtonDown(0, ES_BUTTON_SHIFT))
	{
		MDFNI_SetSetting(Setting.name, Setting.desc->default_value);
		return false;
	}
	else if(es_input->ButtonDown(0, ES_BUTTON_TAB))
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

	if(!es_input->ButtonPressed(0, ES_BUTTON_LEFT) && !es_input->ButtonPressed(0, ES_BUTTON_RIGHT))
	{
		SummerfaceItem_Ptr selected = List->GetSelected();
		bool output = AnchoredListView::Input();

		if(selected != List->GetSelected())
		{
			RefreshHeader = true;
			DoHeaderRefresh();
		}

		return output;
	}
	else
	{
		return false;
	}
}


								SettingMenu::SettingMenu						(const std::string& aDefaultCategory) :
	List(boost::make_shared<SummerfaceList>(Area(10, 10, 80, 80))),
	CategoryList(boost::make_shared<SummerfaceList>(Area(10, 10, 80, 80))),
	Interface(Summerface::Create("Categories", CategoryList))
{
	CategoryList->SetView(boost::make_shared<AnchoredListView>(CategoryList));
	CategoryList->SetHeader("Choose Setting Category");
	List->SetView(boost::make_shared<SettingLineView>(List));

	LoadSettings();

	for(SettingCollection::iterator i = Settings.begin(); i != Settings.end(); i ++)
	{
		SummerfaceItem_Ptr item = boost::make_shared<SummerfaceItem>(TranslateCategory(i->first.c_str()), "");
		item->Properties["CATEGORY"] = i->first;
		CategoryList->AddItem(item);
	}

	CategoryList->Sort();
	CategoryList->SetSelection(TranslateCategory(aDefaultCategory.c_str()));
}

void							SettingMenu::Do									()
{
	while(!WantToDie())
	{
		Interface->Do();

		if(CategoryList->WasCanceled())
		{
			return;
		}
		else
		{
			List->ClearItems();

			std::vector<const MDFNCS*> items = Settings[CategoryList->GetSelected()->Properties["CATEGORY"]];
			for(int i = 0; i != items.size(); i ++)
			{
				SummerfaceItem_Ptr item = boost::make_shared<SummerfaceItem>(items[i]->name, "");
				item->IntProperties["MDFNCS"] = (uint64_t)items[i];
				List->AddItem(item);
			}

			List->Sort(CompareItems);

			Summerface::Create("SettingList", List)->Do();
		}
	}
}

void							SettingMenu::LoadSettings						()
{
	const std::multimap<uint32_t, MDFNCS>* settings = MDFNI_GetSettings();

	for(std::multimap<uint32, MDFNCS>::const_iterator iter = settings->begin(); iter != settings->end(); iter++)
	{
		std::string header = "general";
	
		//HACK: Don't use filesys.*samedir
		if(strstr(iter->second.name, "filesys.") != 0 && strstr(iter->second.name, "samedir") != 0)
		{
			MDFNI_SetSettingB(iter->second.name, false);
			continue;
		}
	
		if(std::string(iter->second.name).find(".esinput.") != std::string::npos || std::string(iter->second.name).find(".bookmarks") != std::string::npos)
		{
			continue;
		}
	
		if(std::string(iter->second.name).find(".") != std::string::npos)
		{
			header = std::string(iter->second.name).substr(0, std::string(iter->second.name).find_first_of("."));
		}

		Settings[header].push_back(&iter->second);
	}
}

std::string						SettingMenu::TranslateCategory					(const char* aCategoryName)
{
	for(int i = 0; i != MDFNSystems.size(); i ++)
	{
		if(strcmp(aCategoryName, MDFNSystems[i]->shortname) == 0)
		{
			return MDFNSystems[i]->fullname;
		}
	}

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

	return aCategoryName;
}
