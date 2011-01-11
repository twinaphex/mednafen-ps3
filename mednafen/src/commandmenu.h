#ifndef MDFN_COMMAND_MENU_H
#define MDFN_COMMAND_MENU_H

class												MednafenCommandItem : public GridItem
{
	public:
													MednafenCommandItem				(const std::string& aDisplay, const std::string& aIcon, const char* aCommand);
		virtual										~MednafenCommandItem			(){};

		bool										Input							();

	protected:
		char										Command[128];
};

class												MednafenCommands : public WinterfaceIconGrid
{
	public:
													MednafenCommands				();
		virtual										~MednafenCommands				(){};
};

#endif
