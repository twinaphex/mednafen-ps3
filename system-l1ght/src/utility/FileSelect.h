#ifndef SYSTEM__FILESELECT_H
#define SYSTEM__FILESELECT_H

class												FileListItem : public ListItem
{
	public:
													FileListItem					(std::string aPath, std::string aName, bool aDirectory, bool aBookMarked = false);

		std::string									GetPath							();
		
		bool										GetDirectory					();
		void										SetBookMark						(bool aSet);
		bool										GetBookMark						();
				
	protected:
		std::string									FullPath;
		bool										Directory;
		bool										BookMark;
};

class												FileList : public WinterfaceList
{
	public:
													FileList						(std::string aHeader, std::vector<std::string>& aBookMarks);
													FileList						(std::string aHeader, std::string aPath, std::vector<std::string>& aBookMarks, MenuHook* aInputHook = 0);
													
		bool										Input							();
		
		std::string									GetFile							();

	protected:
		void										CollectFiles					(std::string aPath, std::vector<ListItem*>& aItems, std::vector<std::string> aFilters);
		void										CollectBookMarks				(std::vector<ListItem*>& aItems);
	
		std::string									Path;
		std::vector<std::string>&					BookMarks;
};

namespace FileSelect
{
	std::string										GetFile							(std::string aHeader, std::vector<std::string>& aBookMarks, MenuHook* aInputHook = 0);
}

#endif
