#include <mednafen_includes.h>

namespace								MednafenSettings
{
	bool								CompareItems									(SummerfaceItem* a, SummerfaceItem* b)
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

	typedef std::map<std::string, std::vector<const MDFNCS*> >	SettingCollection;

	class								SettingLineModel : public LineListModel
	{
		public:
										SettingLineModel								(SummerfaceList* aList) : LineListModel(aList){RefreshHeader = true;}
			virtual						~SettingLineModel								(){}

			void						DoHeaderRefresh									()
			{
				if(RefreshHeader)
				{
					const MDFNCS& Setting = *(const MDFNCS*)List->GetSelected()->IntProperties["MDFNCS"];
					RefreshHeader = false;
					List->SetHeader(Setting.desc->description);
				}
			}

			virtual bool				DrawItem										(SummerfaceItem* aItem, uint32_t aX, uint32_t aY, bool aSelected)
			{
				LineListModel::DrawItem(aItem, aX, aY, aSelected);

				const MDFNCS* setting = (const MDFNCS*)aItem->IntProperties["MDFNCS"];

				if(setting)
				{
					char buffer[256];

					if(setting->desc->type == MDFNST_BOOL)
					{
						    snprintf(buffer, 250, "%s", MDFN_GetSettingB(setting->name) ? "ON" : "OFF");
					}
					else
					{
						    snprintf(buffer, 250, "%s", MDFN_GetSettingS(setting->name).c_str());
					}
					
//HACK: Use real font
				  //  List->LabelFont->PutString(buffer, aX + (es_video->GetClip().Width / 3) * 2, aY, aSelected ? aItem->GetHighLightColor() : aItem->GetNormalColor());
					FontManager::GetBigFont()->PutString(buffer, aX + (es_video->GetClip().Width / 3) * 2, aY, aSelected ? aItem->GetHighLightColor() : aItem->GetNormalColor());

				}
				return false;
			}

			virtual bool				Input											()
			{
				const MDFNCS& Setting = *(const MDFNCS*)List->GetSelected()->IntProperties["MDFNCS"];

				DoHeaderRefresh();

				{
					if(Setting.desc->type == MDFNST_BOOL)
					{
						if(es_input->ButtonDown(0, ES_BUTTON_LEFT) || es_input->ButtonDown(0, ES_BUTTON_RIGHT))
						{
							MDFNI_SetSettingB(Setting.name, MDFN_GetSettingB(Setting.name) == 0);
							return false;
						}
					}
					else if(Setting.desc->type == MDFNST_UINT || Setting.desc->type == MDFNST_INT)
					{
						int32_t value = es_input->ButtonPressed(0, ES_BUTTON_LEFT) ? -1 : 0;
						value = es_input->ButtonPressed(0, ES_BUTTON_RIGHT) ? 1 : value;

						if(value != 0)
						{
						    if(Setting.desc->type == MDFNST_UINT)
						    {
						    	MDFNI_SetSettingUI(Setting.name, MDFN_GetSettingUI(Setting.name) + value);
							}
							else
							{
								char buffer[256];
								snprintf(buffer, 256, "%lli", (long long int)MDFN_GetSettingI(Setting.name) + value);
								MDFNI_SetSetting(Setting.name, buffer);
							}

							return false;
						}
					}
					else if(Setting.desc->type == MDFNST_ENUM)
					{
						int32_t value = es_input->ButtonDown(0, ES_BUTTON_LEFT) ? -1 : 0;
						value = es_input->ButtonDown(0, ES_BUTTON_RIGHT) ? 1 : value;

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
							return false;
						}
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
						FileSelect browse("Select File", nomarks, "file:/");
						    
						std::string result = browse.GetFile();
						    
						if(result.length() != 0)
						{
							if(result.find("file:/") == 0)
							{
								result = result.substr(5);
							}

							MDFNI_SetSetting(Setting.name, result.c_str());
						}

						return false;
					}

					if(!es_input->ButtonPressed(0, ES_BUTTON_LEFT) && !es_input->ButtonPressed(0, ES_BUTTON_RIGHT))
					{
						SummerfaceItem* selected = List->GetSelected();
						bool output = LineListModel::Input();

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
			}

			bool RefreshHeader;

	};

	static std::string					TranslateCategory								(const char* aCategoryName)
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

	static void							GetCategories									(SettingCollection& aSettings)
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

			aSettings[header].push_back(&iter->second);
		}
	}

	static void							DoCategory										(std::vector<const MDFNCS*>& aSettings)
	{
		SummerfaceList* settingList = new SummerfaceList(Area(10, 10, 80, 80));
		settingList->SetModel(new SettingLineModel(settingList));
	
		for(int i = 0; i != aSettings.size(); i ++)
		{
			SummerfaceItem* item = new SummerfaceItem(aSettings[i]->name, "");
			item->IntProperties["MDFNCS"] = (uint64_t)aSettings[i];
			settingList->AddItem(item);
		}

		settingList->Sort(CompareItems);

		Summerface("SettingList", settingList).Do();
	}

	void								Do												(const std::string& aDefaultCategory)
	{
		SettingCollection settings;
		GetCategories(settings);

		SummerfaceList*	cats = new SummerfaceList(Area(10, 10, 80, 80));
		cats->SetHeader("Choose Setting Category");
		for(SettingCollection::iterator i = settings.begin(); i != settings.end(); i ++)
		{
			SummerfaceItem* item = new SummerfaceItem(TranslateCategory(i->first.c_str()), "");
			item->Properties["CATEGORY"] = i->first;
			cats->AddItem(item);
		}
		cats->Sort();
		cats->SetSelection(TranslateCategory(aDefaultCategory.c_str()));

		Summerface settingsface("Categories", cats);
		while(!WantToDie())
		{
			settingsface.Do();

			if(!cats->WasCanceled())
			{
				DoCategory(settings[cats->GetSelected()->Properties["CATEGORY"]]);
			}
			else
			{
				return;
			}
		}
	}
}


