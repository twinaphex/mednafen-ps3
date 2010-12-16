#include <mednafen_includes.h>

namespace
{
	bool							CompareItems									(ListItem* a, ListItem* b)
	{
		if(strstr(a->GetText(), ".enable") != 0)						return true;
		if(strstr(b->GetText(), ".enable") != 0)						return false;		
		if(strstr(a->GetText(), ".ps3.") != 0)							return true;
		if(strstr(b->GetText(), ".ps3.") != 0)							return false;		

		return strcmp(a->GetText(), b->GetText()) <= 0;
	}
}


const char*							MednafenSettingItem::GetDescription				()
{
	return Setting.desc->description;
}

void								MednafenSettingItem::Draw						(uint32_t aX, uint32_t aY, bool aSelected)
{
	ListItem::Draw(aX, aY, aSelected);
	
	char buffer[256];
	
	if(Setting.desc->type == MDFNST_BOOL)
	{
		snprintf(buffer, 250, "%s", MDFN_GetSettingB(Setting.name) ? "ON" : "OFF");
	}
	else 
	{
		snprintf(buffer, 250, "%s", MDFN_GetSettingS(Setting.name).c_str());
	}

	FontManager::GetBigFont()->PutString(buffer, aX + (PS3Video::GetClip().Width / 3) * 2, aY, aSelected ? Colors::HighLight : Colors::Normal);				
}

bool								MednafenSettingItem::Input						()
{
	if(Setting.desc->type == MDFNST_BOOL)
	{
		if(PS3Input::ButtonDown(0, PS3_BUTTON_LEFT) || PS3Input::ButtonDown(0, PS3_BUTTON_RIGHT))
		{
			MDFNI_SetSettingB(Setting.name, MDFN_GetSettingB(Setting.name) == 0);
		}
	}
	else if(Setting.desc->type == MDFNST_UINT || Setting.desc->type == MDFNST_INT)
	{
		int32_t value = PS3Input::ButtonDown(0, PS3_BUTTON_LEFT) ? -1 : 0;
		value = PS3Input::ButtonDown(0, PS3_BUTTON_RIGHT) ? 1 : value;

		if(value != 0)
		{
			MDFNI_SetSettingUI(Setting.name, (Setting.desc->type == MDFNST_INT ? MDFN_GetSettingI(Setting.name) : MDFN_GetSettingUI(Setting.name)) + value);
		}
	}
	else if(Setting.desc->type == MDFNST_ENUM)
	{
		int32_t value = PS3Input::ButtonDown(0, PS3_BUTTON_LEFT) ? -1 : 0;
		value = PS3Input::ButtonDown(0, PS3_BUTTON_RIGHT) ? 1 : value;

		if(value != 0)
		{
			std::string oldvalue = MDFN_GetSettingS(Setting.name);
			uint32_t oldvaluei = 0;
			
			while(1)
			{
				if(Setting.desc->enum_list[oldvaluei].string == 0)
				{
					return false;
				}

				if(strcasecmp(Setting.desc->enum_list[oldvaluei].string, oldvalue.c_str()) == 0)
				{
					break;
				}
				
				oldvaluei ++;
			}
		
			int32_t newvalue = oldvaluei + value;
			
			if(newvalue < 0)
			{
				return false;
			}
			
			for(int i = 0; i != newvalue + 1; i ++)
			{
				if(Setting.desc->enum_list[i].string == 0)
				{
					return false;
				}
			}
			
			MDFNI_SetSetting(Setting.name, Setting.desc->enum_list[newvalue].string);
		}
	}

	if(PS3Input::ButtonDown(0, PS3_BUTTON_CROSS))
	{
		std::string result;
		Keyboard kb(Setting.name, MDFN_GetSettingS(Setting.name));
		kb.Do();
		
		if(!kb.WasCanceled())
		{
			MDFNI_SetSetting(Setting.name, kb.GetText().c_str());
		}
	}
	else if(PS3Input::ButtonDown(0, PS3_BUTTON_TRIANGLE))
	{
		MDFNI_SetSetting(Setting.name, Setting.desc->default_value);
	}
	else if(PS3Input::ButtonDown(0, PS3_BUTTON_L2))
	{
		std::vector<std::string> nomarks;
		FileSelect browse("Select File", nomarks);
		
		std::string result = browse.GetFile();
		
		if(result.length() != 0)
		{
			MDFNI_SetSetting(Setting.name, result.c_str());
		}
	}

	return ListItem::Input();
}

									MednafenSettings::MednafenSettings				(const std::string& aDefault) : WinterfaceMultiList("Emulator Settings", false, true, 0)
{
	const std::multimap<uint32_t, MDFNCS>* settings = MDFNI_GetSettings();

	for(std::multimap<uint32, MDFNCS>::const_iterator iter = settings->begin(); iter != settings->end(); iter++)
	{
		std::string header = "general";
		
		if(std::string(iter->second.name).find(".ps3input.") != std::string::npos)
		{
			continue;
		}
		
		if(std::string(iter->second.name).find(".bookmarks") != std::string::npos)
		{
			continue;
		}
		
		if(std::string(iter->second.name).find(".") != std::string::npos)
		{
			header = std::string(iter->second.name).substr(0, std::string(iter->second.name).find_first_of("."));
		}

		Categories[header].push_back(new MednafenSettingItem(iter->second, iter->second.name));
	}

	for(std::map<std::string, std::vector<ListItem*> >::iterator cat = Categories.begin(); cat != Categories.end(); cat ++)
	{
		std::sort(cat->second.begin(), cat->second.end(), CompareItems);
	}
	
	SetCategory(aDefault);
	
	SideItems.push_back(new ListItem("[DPAD] Navigate", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[DPAD] Change Value", FontManager::GetSmallFont()));	
	SideItems.push_back(new ListItem("[X] Use Keyboard", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[T] Reset to Default", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[R1] Change Category", FontManager::GetSmallFont()));	
	SideItems.push_back(new ListItem("[L1] Change Category", FontManager::GetSmallFont()));	
	SideItems.push_back(new ListItem("[L2] Set to File", FontManager::GetSmallFont()));			
	SideItems.push_back(new ListItem("[O] Close", FontManager::GetSmallFont()));	
}

