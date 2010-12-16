#ifndef MDFN_COMMAND_MENU_H
#define MDFN_COMMAND_MENU_H

class												MednafenCommandItem : public GridItem
{
	public:
													MednafenCommandItem				(const char* aDisplay, const char* aIcon, const char* aCommand);
		virtual										~MednafenCommandItem			();
													
		bool										Input							();

	protected:
		char*										Command;
};

class												MednafenCommands : public WinterfaceIconGrid
{
	public:
													MednafenCommands				();
};

#endif
