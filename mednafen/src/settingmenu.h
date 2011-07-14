#pragma once

///ListItem class for settings.
struct								SettingItem
{
									SettingItem							(const MDFNCS* aSetting) : Setting(aSetting){}

	std::string						GetText								() {return std::string(Setting->name) + "\t" + ((Setting->desc->type == MDFNST_BOOL) ? (MDFN_GetSettingB(Setting->name) ? "ON" : "OFF") : MDFN_GetSettingS(Setting->name));}
	std::string						GetImage							() {return "";}

	uint32_t						GetNormalColor						() {return Colors::Normal;}
	uint32_t						GetHighLightColor					() {return Colors::HighLight;}

	const MDFNCS*					Setting;
};


///Class used to list and edit mednafen's settings.
class								SettingMenu
{
	typedef std::map<std::string, std::vector<const MDFNCS*> >	SettingCollection;	///<Collection type for the Settings cache

	typedef SummerfaceItemUser<std::string>						CategoryListItem;
	typedef smartptr::shared_ptr<CategoryListItem>				CategoryListItem_Ptr;
	typedef AnchoredListView<CategoryListItem>					CategoryListType;
	typedef smartptr::shared_ptr<CategoryListType>				CategoryListType_Ptr;

	typedef AnchoredListView<SettingItem>						SettingListType;
	typedef smartptr::shared_ptr<SettingListType>				SettingListType_Ptr;

	public:
		///Create a new SettingMenu.
		///@param aDefaultCategory The name of the default category. This category will be selected at load, and shaded in a special color.
									SettingMenu							(const std::string& aDefaultCategory = std::string());

		///Blank virtual destructor for SetttingMenu.
		virtual						~SettingMenu						() {}

		///Run the setting editor.
		void						Do									();

		int							HandleInput							(Summerface_Ptr aInterface, const std::string& aWindow, uint32_t aButton);

		///Refresh the parent list's header if needed.
		void						DoHeaderRefresh						();

		///Handle changing a boolean setting.
		///@param aButton System button that was pressed.
		///@param aSetting Setting to update.
		///@return True if this function has eaten the input, false to continue processing.
		bool						HandleBool							(uint32_t aButton, const MDFNCS& aSetting);

		///Handle changing an int or unsigned int setting.
		///@param aButton System button that was pressed.
		///@param aSetting Setting to update.
		///@return True if this function has eaten the input, false to continue processing.
		bool						HandleInt							(uint32_t aButton, const MDFNCS& aSetting);

		///Handle changing an enumeration setting.
		///@param aButton System button that was pressed.
		///@param aSetting Setting to update.
		///@return True if this function has eaten the input, false to continue processing.
		bool						HandleEnum							(uint32_t aButton, const MDFNCS& aSetting);

	private:
		///Load and cache all of the settings from the mednafen core.
		void						LoadSettings						();

		///Translate a sparse category name to a friendly one.
		std::string					TranslateCategory					(const char* aCategory);

	private:
		SettingListType_Ptr			List;								///<SummerfaceList used for displaying a category of settings.
		CategoryListType_Ptr		CategoryList;						///<SummerfaceList used for displaying all categories.
		Summerface_Ptr				Interface;							///<Summerface object used for the CategoryList.

		SettingCollection			Settings;							///<The cache of settings from mednafen.

		bool						RefreshHeader;						///<True if the header needs to be refreshed.
};

