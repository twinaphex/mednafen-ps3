#include <ps3_system.h>

					ErrorDialog::ErrorDialog				(const std::string& aMessage, const std::string& aHeader) : Winterface(aHeader, true, 0)
{
	Message = aMessage;

	SideItems.push_back(new InputListItem("Continue", ES_BUTTON_ACCEPT));	
}
							
bool				ErrorDialog::Input						()
{
	if(es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
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
