#ifndef MDFN_SETTING_MENU_H
#define MDFN_SETTING_MENU_H

class												MednafenSettingItem : public ListItem
{
	public:
													MednafenSettingItem				(MDFNCS aSetting, const char* aDisplay) : Setting(aSetting), ListItem(aDisplay){};
													~MednafenSettingItem			(){};
													
		void										Draw							(uint32_t aX, uint32_t aY, bool aSelected);
		bool										Input							();
		
		const char*									GetDescription					();
	
	protected:
		MDFNCS										Setting;
};

class												MednafenSettings : public WinterfaceMultiList
{
	public:
													MednafenSettings				(const std::string& aDefault);
};

#endif