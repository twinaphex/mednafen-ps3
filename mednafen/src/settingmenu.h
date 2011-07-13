#pragma once

class								SettingLineView : public AnchoredListView
{
	public:
									SettingLineView						(SummerfaceList_WeakPtr aList) : AnchoredListView(aList), RefreshHeader(true) {}
		virtual						~SettingLineView					() {}

		bool						Input								(uint32_t aButton);

		void						DoHeaderRefresh						();
		virtual bool				DrawItem							(SummerfaceList_Ptr aList, SummerfaceItem_Ptr aItem, uint32_t aX, uint32_t aY, bool aSelected);

		bool						HandleBool							(uint32_t aButton, const MDFNCS& aSetting);
		bool						HandleInt							(uint32_t aButton, const MDFNCS& aSetting);
		bool						HandleEnum							(uint32_t aButton, const MDFNCS& aSetting);

	private:
		bool RefreshHeader;
};

typedef std::map<std::string, std::vector<const MDFNCS*> >	SettingCollection;


class								SettingMenu
{
	public:
									SettingMenu							(const std::string& aDefaultCategory = std::string());
		virtual						~SettingMenu						() {}

		void						Do									();

	private:
		void						LoadSettings						();
		std::string					TranslateCategory					(const char* aCategory);

	private:
		SummerfaceList_Ptr			List;
		SummerfaceList_Ptr			CategoryList;
		Summerface_Ptr				Interface;

		SettingCollection			Settings;
};

