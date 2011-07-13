#ifndef SYSTEM__FILESELECT_H
#define SYSTEM__FILESELECT_H

typedef std::vector<std::string>					BookmarkList;

class												FileSelect
{
	public:
													FileSelect						(const std::string& aHeader, BookmarkList& aBookMarks, const std::string& aPath, SummerfaceInputConduit_Ptr aInputHook = SummerfaceInputConduit_Ptr());
		virtual										~FileSelect						() {};
													
		int											HandleInput						(Summerface_Ptr aInterface, const std::string& aWindow, uint32_t aButton);

		std::string									GetFile							();

	private:
		void										LoadList						(const std::string& aPath);
		SummerfaceItem_Ptr							MakeItem						(const std::string& aName, const std::string& aPath, bool aDirectory, bool aFile);

	private:
		SummerfaceList_Ptr							List;
		Summerface_Ptr								Interface;
		std::stack<std::string>						Paths;

		std::string									Header;
		BookmarkList&								BookMarks;
};

#endif

