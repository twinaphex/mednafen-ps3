#include <mednafen_includes.h>

bool								MednafenCommandItem::Input						()
{
	if(PS3Input::ButtonDown(0, PS3_BUTTON_CROSS))
	{
		MednafenEmu::DoCommand(Command);
		return true;
	}
	
	return false;
}

									MednafenCommands::MednafenCommands				() : WinterfaceIconGrid("Emulator Commands", 5, 2, false)
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
	Items.push_back(new MednafenCommandItem("Exit Mednafen", "DoExit", "DoExit"));

	SideItems.push_back(new ListItem("[DPAD] Navigate", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[X] Run Command", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[O] Close", FontManager::GetSmallFont()));	
}
