#ifndef SYSTEM__FILELIST_H
#define SYSTEM__FILELIST_H

class												FileList : public SummerfaceLineList
{
	public:
													FileList						(const Area& aRegion, const std::string& aPath, std::vector<std::string>& aBookmarks);
		virtual										~FileList						(){};
													
		bool										Input							();

		std::string									GetFile							();
		bool										IsDirectory						();

		static SummerfaceItem*						MakeItem						(const std::string& aName, const std::string& aPath, bool aDirectory, bool aFile, bool aBookMark);


	protected:
		std::string									Path;
		std::vector<std::string>&					BookMarks;
};

#endif
