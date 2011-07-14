#pragma once

///ListView subclass for handling a SettingLineView's input.
class								SettingLineView : public AnchoredListView<SummerfaceItem>
{
	public:
		///Create a new SettingLineView.
		///@param aList Weak pointer to the view's parent SummerfaceList.
									SettingLineView						(const Area& aRegion) : AnchoredListView(aRegion), RefreshHeader(true) {}

		///Blank virtual destructor for SettingLineView.
		virtual						~SettingLineView					() {}

		///Implement an Input method for ListView.
		///@param aButton System button that was pressed.
		///@return True to stop processing the interface.
		bool						Input								(uint32_t aButton);

		///Refresh the parent list's header if needed.
		///@param aList The parent list.
		void						DoHeaderRefresh						();

		///Implement a DrawItem method for AnchoredListView.
		///@param aList Parent list.
		///@param aItem Item to draw.
		///@param aX X position to draw the item.
		///@param aY Y position to draw the item.
		///@param aSelected True if this item is the currently selected item.
		virtual bool				DrawItem							(SummerfaceItem_Ptr aItem, uint32_t aX, uint32_t aY, bool aSelected);

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
		bool						RefreshHeader;						///<When true DoHeaderRefresh will update the parent interfaces header with the currently selected item.
};

///Class used to list and edit mednafen's settings.
class								SettingMenu
{
	typedef std::map<std::string, std::vector<const MDFNCS*> >	SettingCollection;	///<Collection type for the Settings cache

	public:
		///Create a new SettingMenu.
		///@param aDefaultCategory The name of the default category. This category will be selected at load, and shaded in a special color.
									SettingMenu							(const std::string& aDefaultCategory = std::string());

		///Blank virtual destructor for SetttingMenu.
		virtual						~SettingMenu						() {}

		///Run the setting editor.
		void						Do									();

	private:
		///Load and cache all of the settings from the mednafen core.
		void						LoadSettings						();

		///Translate a sparse category name to a friendly one.
		std::string					TranslateCategory					(const char* aCategory);

	private:
		smartptr::shared_ptr<SettingLineView>		List;								///<SummerfaceList used for displaying a category of settings.
		smartptr::shared_ptr<AnchoredListView<SummerfaceItem> >			CategoryList;						///<SummerfaceList used for displaying all categories.
		Summerface_Ptr				Interface;							///<Summerface object used for the CategoryList.

		SettingCollection			Settings;							///<The cache of settings from mednafen.
};

