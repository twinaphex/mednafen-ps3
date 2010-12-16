#ifndef MDFN_COMMAND_MENU_H
#define MDFN_COMMAND_MENU_H

class												MednafenCommandItem : public GridItem
{
	public:
													MednafenCommandItem				(std::string aDisplay, std::string aIcon, std::string aCommand) : GridItem(aDisplay, aIcon), Command(aCommand){};
		virtual										~MednafenCommandItem			(){};
													
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