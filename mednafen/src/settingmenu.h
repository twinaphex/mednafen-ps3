#ifndef MDFN_SETTING_MENU_H
#define MDFN_SETTING_MENU_H

class												MednafenSettingItem : public ListItem
{
	public:
													MednafenSettingItem				(MDFNCS aSetting, std::string aDisplay) : Setting(aSetting), ListItem(aDisplay){};
													~MednafenSettingItem			(){};
													
		void										Draw							(uint32_t aX, uint32_t aY, bool aSelected);
		bool										Input							();
		
		std::string									GetDescription					();
	
	protected:
		MDFNCS										Setting;
};

class												MednafenSettings : public WinterfaceMultiList
{
	public:
													MednafenSettings				(std::string aDefault);
		std::string									GetHeader						();
};

#endif