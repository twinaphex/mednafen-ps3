#pragma once

///ListItem class for settings.
struct								SettingItem
{
	///Create a new SettingItem.
	///@param aSetting Pointer to setting data. May not be NULL.
									SettingItem							(const MDFNCS* aSetting, const std::string& aGroup) : Setting(aSetting), Group(aGroup) {assert(aSetting);}

	///Return a string containing the settings name and value.
	///@return The settings name and value, separated by a tab character.
	std::string						GetText								(); //External

	///Returns an empty string.
	///@return An empty string.
	std::string						GetImage							() {return "";}

	///Return the group for GroupListView
	///@return The group name.
	std::string						GetGroup							() {return Group;}

	///Retruns Colors::Normal.
	///@return Colors::Normal.
	uint32_t						GetNormalColor						() {return Colors::Normal;}

	///Retruns Colors::HighLight.
	///@return Colors::HighLight.
	uint32_t						GetHighLightColor					() {return Colors::HighLight;}

	const MDFNCS*					Setting;							///<A pointer to the managed setting.
	std::string						Group;								///<The group name for GroupListView.
};


///Class used to list and edit mednafen's settings.
class								SettingMenu
{
	typedef std::map<std::string, std::vector<const MDFNCS*> >	SettingCollection;	///<Collection type for the Settings cache

	typedef SummerfaceItemUser<std::string>						CategoryListItem;
	typedef smartptr::shared_ptr<CategoryListItem>				CategoryListItem_Ptr;
	typedef AnchoredListView<CategoryListItem>					CategoryListType;
	typedef smartptr::shared_ptr<CategoryListType>				CategoryListType_Ptr;

	typedef GroupListView<SettingItem>							SettingListType;
	typedef smartptr::shared_ptr<SettingListType>				SettingListType_Ptr;

	typedef AnchoredListView<SummerfaceItem>					EnumListType;
	typedef smartptr::shared_ptr<EnumListType>					EnumListType_Ptr;

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

		bool						HandleButton						(uint32_t aButton, const MDFNCS& aSetting);

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

		///Translate a setting into a group name.
		///@param aSyatem System name of the setting, or an empty string for a general setting.
		///@return A group name.
		std::string					TranslateGroup						(const MDFNCS& aSetting, const std::string& aSystem);

		///Translate a sparse category name to a friendly one.
		std::string					TranslateCategory					(const char* aCategory);

	private:
		SettingListType_Ptr			List;								///<SummerfaceList used for displaying a category of settings.
		CategoryListType_Ptr		CategoryList;						///<SummerfaceList used for displaying all categories.
		Summerface_Ptr				CategoryInterface;					///<Summerface object used for the CategoryList.

		SettingCollection			Settings;							///<The cache of settings from mednafen.

		bool						RefreshHeader;						///<True if the header needs to be refreshed.
};

