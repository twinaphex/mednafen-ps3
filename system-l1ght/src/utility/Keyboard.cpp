#include <ps3_system.h>

namespace
{
	const char*		Chars[2][5] = {{"`1234567890-=", "qwertyuiop[]\\", "asdfghjkl;'", "zxcvbnm,./", " "}, {"!@#$%^&*()_+", "QWERTYUIOP{}|", "ASDFGHJKL:\"", "ZXCVBNM<>?", " "}};
}

							Keyboard::Keyboard								(const std::string& aHeader, const std::string& aText) : Winterface(aHeader)
{
	Text.reserve(256);

	Text = aText;
	Header = aHeader;

	Column = 0;
	Row = 0;
	Shift = 0;
	Canceled = false;
	
	SideItems.push_back(new ListItem("[DPAD] Move Selection", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[X] Input Character", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[T] Shift", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[S] Backspace", FontManager::GetSmallFont()));	
	SideItems.push_back(new ListItem("[O] Finish", FontManager::GetSmallFont()));
}

							Keyboard::~Keyboard								()
{
							
}

							
//TODO: This code can be dangerous
bool						Keyboard::DrawLeft								()
{
	FontManager::GetBigFont()->PutString(Header, 16, 0, Colors::Normal);
	FontManager::GetBigFont()->PutString(Text, 16, FontManager::GetBigFont()->GetHeight() + 2, Colors::HighLight);
	
	uint32_t startX = 0;
	
	for(int i = 0; i != 4; i ++)
	{
		for(int j = 0; j != strlen(Chars[Shift][i]); j ++)
		{
			char charr[2] = {Chars[Shift][i][j], 0};
			FontManager::GetBigFont()->PutString(charr, startX + (j * 2) * FontManager::GetBigFont()->GetWidth(), (i + 4) * FontManager::GetBigFont()->GetHeight(), (i == Row && j == Column) ? Colors::HighLight : Colors::Normal);
		}
	}

	FontManager::GetBigFont()->PutString("     [SPACE]", startX, (4 + 4) * FontManager::GetBigFont()->GetHeight(), (Row == 4) ? Colors::HighLight : Colors::Normal);			

	return false;	
}

//TODO: This code can be dangerous
bool						Keyboard::Input									()
{
	Row += PS3Input::ButtonDown(0, PS3_BUTTON_DOWN) ? 1 : 0;
	Row -= PS3Input::ButtonDown(0, PS3_BUTTON_UP) ? 1 : 0;
	Row = Utility::Clamp(Row, 0, 4);
	
	Column += PS3Input::ButtonDown(0, PS3_BUTTON_RIGHT) ? 1 : 0;
	Column -= PS3Input::ButtonDown(0, PS3_BUTTON_LEFT) ? 1 : 0;
	Column = Utility::Clamp(Column, 0, strlen(Chars[0][Row]) - 1);

	if(PS3Input::ButtonDown(0, PS3_BUTTON_CROSS))
	{
		Text.push_back(Chars[Shift][Row][Column]);
	}
	else if(PS3Input::ButtonDown(0, PS3_BUTTON_TRIANGLE))
	{
		Shift = Shift == 0 ? 1 : 0;
	}
	else if(PS3Input::ButtonDown(0, PS3_BUTTON_SQUARE))
	{
		if(!Text.empty())
		{
			Text.erase(Text.length() - 1);
		}
	}
	else if(PS3Input::ButtonDown(0, PS3_BUTTON_CIRCLE))
	{
		return true;
	}
		
	return false;
}
