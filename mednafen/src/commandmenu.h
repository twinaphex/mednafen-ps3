#ifndef MDFN_COMMAND_MENU_H
#define MDFN_COMMAND_MENU_H

class												MednafenCommandItem : public ListItem
{
	public:
													MednafenCommandItem				(std::string aDisplay, std::string aCommand) : ListItem(aDisplay), Command(aCommand){};
		virtual										~MednafenCommandItem			(){};
													
		bool										Input							();

	protected:
		std::string									Command;
};

class												MednafenCommands : public WinterfaceList
{
	public:
													MednafenCommands				();
};

#endif