#ifndef SYSTEM__FILESELECT_H
#define SYSTEM__FILESELECT_H

class												FileSelect
{
	public:
													FileSelect						(const std::string& aHeader, std::vector<std::string>& aBookMarks, const std::string& aPath, MenuHook* aInputHook = 0);
													~FileSelect						();
													
		std::string									GetFile							();

	protected:
		std::stack<FileList*>						Lists;

		std::string									Header;
		std::vector<std::string>&					BookMarks;
		MenuHook*									InputHook;
};

#endif
