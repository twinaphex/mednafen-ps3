#ifndef SYSTEM__FILESELECT_H
#define SYSTEM__FILESELECT_H

class												FileListItem : public ListItem
{
	public:
													FileListItem					(const std::string& aPath, const std::string& aName, bool aDirectory, bool aBookMarked = false);

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
													FileList						(const std::string& aHeader, const std::string& aPath, std::vector<std::string>& aBookMarks, MenuHook* aInputHook = 0);
													
		bool										Input							();
		
		std::string									GetFile							();

	protected:
		void										CollectFiles					(const std::string& aPath, std::vector<ListItem*>& aItems, std::vector<std::string> aFilters);
		void										CollectBookMarks				(std::vector<ListItem*>& aItems);
	
		std::string									Path;
		std::vector<std::string>&					BookMarks;
};

class												FileSelect
{
	public:
													FileSelect						(const std::string& aHeader, std::vector<std::string>& aBookMarks, MenuHook* aInputHook = 0);
													~FileSelect						();
		std::string									GetFile							();

	protected:
		std::stack<FileList*>						Lists;

		std::string									Header;
		std::vector<std::string>&					BookMarks;
		MenuHook*									InputHook;
};

#endif
