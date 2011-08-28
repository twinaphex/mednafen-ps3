#pragma once

///Class used to list setting categories.
class								SettingCategoryMenu
{
	typedef std::map<std::string, std::vector<const MDFNCS*> >	SettingCollection;	///<Collection type for the Settings cache

	typedef SummerfaceItemUser<std::string>						ListItem;			///<Type of items in the list.
	typedef AnchoredListView<ListItem>							ListType;			///<Type of the Summerface List widget.

	public:
		///Create a new SettingCategoryMenu.
		///@param aDefaultCategory The name of the default category. This category will be selected at load, and shaded in a special color.
									SettingCategoryMenu					(const std::string& aDefaultCategory = std::string());

		///Blank virtual destructor for SetttingMenu.
		virtual						~SettingCategoryMenu				() {}

		///Run the setting editor.
		void						Do									();

	private:
		///Load and cache all of the settings from the mednafen core.
		void						LoadSettings						();

		///Translate a sparse category name to a friendly one.
		std::string					TranslateCategory					(const char* aCategory);

	private:
		ListType					List;								///<SummerfaceList used for displaying all categories.
		Summerface					Interface;							///<Summerface object used for the List.

		SettingCollection			Settings;							///<The cache of settings from mednafen.
};

