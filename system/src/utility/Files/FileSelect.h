#ifndef SYSTEM__FILESELECT_H
#define SYSTEM__FILESELECT_H

typedef std::vector<std::string>					BookmarkList;

class												FileSelect
{
	public:
		struct										DirectoryItem
		{
													DirectoryItem				(const std::string& aName, const std::string& aPath, bool aDirectory, bool aFile, bool aIsBookMark);

			std::string								GetText						() {return Name;}
			std::string								GetImage					() {return Image;}

			uint32_t								GetNormalColor				() {return IsBookMark ? Colors::SpecialNormal : Colors::Normal;}
			uint32_t								GetHighLightColor			() {return IsBookMark ? Colors::SpecialHighLight : Colors::HighLight;}

			std::string								Name;
			std::string								Extension;
			std::string								Image;
			std::string								Path;
			bool									IsDirectory;
			bool									IsFile;
			bool									IsBookMark;
		};

		typedef smartptr::shared_ptr<DirectoryItem>	DirectoryItem_Ptr;
		typedef AnchoredListView<DirectoryItem>		DirectoryList;

	public:
													FileSelect						(const std::string& aHeader, BookmarkList& aBookMarks, const std::string& aPath, SummerfaceInputConduit_Ptr aInputHook = SummerfaceInputConduit_Ptr());
		virtual										~FileSelect						() {};
													
		int											HandleInput						(Summerface_Ptr aInterface, const std::string& aWindow, uint32_t aButton);

		std::string									GetFile							();

	private:
		void										LoadList						(const std::string& aPath);

	private:
		smartptr::shared_ptr<DirectoryList>			List;
		Summerface_Ptr								Interface;
		std::stack<std::string>						Paths;

		std::string									Header;
		BookmarkList&								BookMarks;
};

#endif

