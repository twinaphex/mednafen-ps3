#ifndef MDFN_COMMAND_MENU_H
#define MDFN_COMMAND_MENU_H

class												MednafenCommandItem : public GridItem
{
	public:
													MednafenCommandItem				(const std::string& aDisplay, const std::string& aIcon, const std::string& aCommand);
		bool										Input							();

	protected:
		std::string									Command;
};

class												MednafenCommands : public WinterfaceIconGrid
{
	public:
													MednafenCommands				();
};

#endif
