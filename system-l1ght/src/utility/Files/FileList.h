#ifndef SYSTEM__FILELIST_H
#define SYSTEM__FILELIST_H

class												FileList : public WinterfaceList
{
	public:
													FileList						(const std::string& aHeader, const std::string& aPath, std::vector<std::string>& aBookMarks, MenuHook* aInputHook = 0);
													
		bool										Input							();

	public:	//Inlines
		std::string									GetFile							()
		{
			return WasCanceled() ? "" : ((FileListItem*)GetSelected())->GetPath();
		}

	protected:
		void										CollectFiles					(const std::string& aPath, std::vector<ListItem*>& aItems, std::vector<std::string> aFilters);
		void										CollectBookMarks				(std::vector<ListItem*>& aItems);
	
		std::string									Path;
		std::vector<std::string>&					BookMarks;
};

#endif
