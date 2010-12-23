#include <ps3_system.h>

					ErrorDialog::ErrorDialog				(const std::string& aMessage, const std::string& aHeader) : Winterface(aHeader, true, 0)
{
	Message = aMessage;
	
	SideItems.push_back(new ListItem("[X] Continue", FontManager::GetSmallFont()));
}
							
bool				ErrorDialog::Input						()
{
	if(PS3Input::ButtonDown(0, PS3_BUTTON_CROSS))
	{
		return true;
	}
	
	return Winterface::Input();
}

bool				ErrorDialog::DrawLeft					()
{
	FontManager::GetBigFont()->PutString(Message.c_str(), 32, 32, Colors::Normal);

	return false;
}
