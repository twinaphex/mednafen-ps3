#include <mednafen_includes.h>

									MednafenCommandItem::MednafenCommandItem		(const std::string& aDisplay, const std::string& aIcon, const std::string& aCommand) : GridItem(aDisplay, aIcon)
{
	Command = aCommand;
}

									MednafenCommandItem::~MednafenCommandItem		()
{
}

bool								MednafenCommandItem::Input						()
{
	if(es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
	{
		MednafenEmu::DoCommand(Command);
		return true;
	}
	
	return false;
}

									MednafenCommands::MednafenCommands				() : WinterfaceIconGrid("Emulator Commands", 3, 3, true)
{
	Items.push_back(new MednafenCommandItem("Change Game", "DoReload", "DoReload"));
	Items.push_back(new MednafenCommandItem("Reset Game", "DoReset", "DoReset"));
	Items.push_back(new MednafenCommandItem("Show Text File", "DoTextFile", "DoTextFile"));	
	Items.push_back(new MednafenCommandItem("Save State", "DoSaveState", "DoSaveState"));
	Items.push_back(new MednafenCommandItem("Load State", "DoLoadState", "DoLoadState"));
	Items.push_back(new MednafenCommandItem("Take Screen Shot", "DoScreenShot", "DoScreenShot"));
	Items.push_back(new MednafenCommandItem("Enable Rewind", "DoToggleRewind", "DoToggleRewind"));	
	Items.push_back(new MednafenCommandItem("Settings", "DoSettings", "DoSettings"));
	Items.push_back(new MednafenCommandItem("Configure Controls", "DoInputConfig", "DoInputConfig"));
//	Items.push_back(new MednafenCommandItem("Exit Mednafen", "DoExit", "DoExit"));

	SideItems.push_back(new ListItem("[DPAD] Navigate", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[X] Run Command", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[O] Close", FontManager::GetSmallFont()));	
}
