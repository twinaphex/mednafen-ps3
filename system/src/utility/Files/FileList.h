#ifndef SYSTEM__FILELIST_H
#define SYSTEM__FILELIST_H

class												FileList : public SummerfaceLineList
{
	public:
													FileList						(const Area& aRegion, const std::string& aHeader, const std::string& aPath, std::vector<std::string>& aBookmarks);
		virtual										~FileList						(){};
													
		bool										Input							();

		std::string									GetFile							();
		bool										IsDirectory						();

	protected:
		std::string									Path;
		std::vector<std::string>&					BookMarks;
};

#endif
