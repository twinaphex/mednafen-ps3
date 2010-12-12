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

									MednafenCommands::MednafenCommands				() : WinterfaceList("Emulator Commands", false, true, 0)
{
	Items.push_back(new MednafenCommandItem("Change Game", "DoReload"));
	Items.push_back(new MednafenCommandItem("Reset Game", "DoReset"));
	Items.push_back(new MednafenCommandItem("Show Text File", "DoTextFile"));	
	Items.push_back(new MednafenCommandItem("Save State", "DoSaveState"));
	Items.push_back(new MednafenCommandItem("Load State", "DoLoadState"));
	Items.push_back(new MednafenCommandItem("Take Screen Shot", "DoScreenShot"));
	Items.push_back(new MednafenCommandItem("Enable Rewind", "DoToggleRewind"));	
	Items.push_back(new MednafenCommandItem("Settings", "DoSettings"));
	Items.push_back(new MednafenCommandItem("Configure Controls", "DoInputConfig"));
	Items.push_back(new MednafenCommandItem("Exit Mednafen", "DoExit"));

	SideItems.push_back(new ListItem("[DPAD] Navigate", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[X] Run Command", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[O] Close", FontManager::GetSmallFont()));	
}
