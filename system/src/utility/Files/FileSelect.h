#ifndef SYSTEM__FILESELECT_H
#define SYSTEM__FILESELECT_H

class												FileSelect
{
	public:
		struct										DirectoryItem
		{
													DirectoryItem				(const std::string& aName, const std::string& aPath, bool aDirectory, bool aFile, bool aIsBookMark);

			std::string								GetText						() {return Name;}
			std::string								GetImage					() {return Image;}

			uint32_t								GetNormalColor				() {return IsBookMark ? Colors::GetColor("bookmark", Colors::green) : Colors::GetColor("text", Colors::black);}
			uint32_t								GetHighLightColor			() {return IsBookMark ? Colors::GetColor("selectedbookmark", Colors::darkgreen) : Colors::GetColor("selectedtext", Colors::red);}

			std::string								Name;
			std::string								Extension;
			std::string								Image;
			std::string								Path;
			bool									IsDirectory;
			bool									IsFile;
			bool									IsBookMark;
		};

		typedef AnchoredListView<DirectoryItem>		DirectoryList;

	public:
		//Doc Note: aInputHook must be a uniqe pointer and will be deleted by the FileSelect object.
													FileSelect						(const std::string& aHeader, const std::string& aBookMarks, const std::string& aPath, SummerfaceInputConduit* aInputHook = 0);
		virtual										~FileSelect						() {};

		int											HandleInput						(Summerface* aInterface, const std::string& aWindow, uint32_t aButton);

		std::string									GetFile							();
		std::string&								GetBookmarks					(std::string& aOutput);

	private:
		void										LoadList						(const std::string& aPath);

	private:
		typedef std::vector<std::string>			BookmarkList;

		DirectoryList								List;
		Summerface									Interface;
		std::stack<std::string>						Paths;

		std::string									Header;
		BookmarkList								BookMarks;
};

#endif

