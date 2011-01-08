#ifndef MDFN_SETTING_MENU_H
#define MDFN_SETTING_MENU_H

class												MednafenSettingItem : public ListItem
{
	public:
													MednafenSettingItem				(MDFNCS aSetting, const std::string& aDisplay) : Setting(aSetting), ListItem(aDisplay){};

		void										Draw							(uint32_t aX, uint32_t aY, bool aSelected);
		bool										Input							();

		std::string									GetDescription					();

	protected:
		MDFNCS										Setting;
};

class												MednafenSettings : public WinterfaceMultiList
{
	public:
													MednafenSettings				(const std::string& aDefault);

		virtual std::string							GetHeader						();
};

#endif


