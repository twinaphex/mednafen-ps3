#ifndef SYSTEM__FILESELECT_H
#define SYSTEM__FILESELECT_H

class												FileSelect
{
	public:
													FileSelect						(const std::string& aHeader, std::vector<std::string>& aBookMarks, const std::string& aPath, MenuHook* aInputHook = 0);
		virtual										~FileSelect						() {};
													
		std::string									GetFile							();
		bool										IsValid							() {return Valid;}

	private:
		void										LoadList						(const char* aPath);
		static SummerfaceItem_Ptr					MakeItem						(const std::string& aName, const std::string& aPath, bool aDirectory, bool aFile, bool aBookMark);

	private:

		SummerfaceList_Ptr							List;
		Summerface_Ptr								Interface;
		std::stack<std::string>						Paths;

		bool										Valid;

		std::string									Header;
		std::vector<std::string>&					BookMarks;
};

#endif
