#ifndef SYSTEM__FILELIST_H
#define SYSTEM__FILELIST_H

class												FileList : public WinterfaceList
{
	public:
													FileList						(const std::string& aHeader, const std::string& aPath, std::vector<std::string>& aBookmarks, MenuHook* aInputHook = 0);
													
		bool										Input							();

	public:	//Inlines
		std::string									GetFile							()
		{
			return WasCanceled() ? "" : ((FileListItem*)GetSelected())->GetPath();
		}

	protected:
		std::string									Path;
		std::vector<std::string>&					BookMarks;
};

#endif
