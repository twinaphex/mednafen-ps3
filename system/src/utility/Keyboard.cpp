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
	SideItems.push_back(new ListItem("[ACCEPT] Input Character", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[SHIFT] Shift", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[TAB] Backspace", FontManager::GetSmallFont()));	
	SideItems.push_back(new ListItem("[CANCEL] Finish", FontManager::GetSmallFont()));
}

							Keyboard::~Keyboard								()
{
							
}

							
//TODO: This code can be dangerous
bool						Keyboard::DrawLeft								()
{
	FontManager::GetBigFont()->PutString(Header.c_str(), 16, 0, Colors::Normal);
	FontManager::GetBigFont()->PutString(Text.c_str(), 16, FontManager::GetBigFont()->GetHeight() + 2, Colors::HighLight);
	
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
	Row += es_input->ButtonDown(0, ES_BUTTON_DOWN) ? 1 : 0;
	Row -= es_input->ButtonDown(0, ES_BUTTON_UP) ? 1 : 0;
	Row = Utility::Clamp(Row, 0, 4);
	
	Column += es_input->ButtonDown(0, ES_BUTTON_RIGHT) ? 1 : 0;
	Column -= es_input->ButtonDown(0, ES_BUTTON_LEFT) ? 1 : 0;
	Column = Utility::Clamp(Column, 0, strlen(Chars[0][Row]) - 1);

	if(es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
	{
		Text.push_back(Chars[Shift][Row][Column]);
	}
	else if(es_input->ButtonDown(0, ES_BUTTON_SHIFT))
	{
		Shift = Shift == 0 ? 1 : 0;
	}
	else if(es_input->ButtonDown(0, ES_BUTTON_TAB))
	{
		if(!Text.empty())
		{
			Text.erase(Text.length() - 1);
		}
	}
	else if(es_input->ButtonDown(0, ES_BUTTON_CANCEL))
	{
		return true;
	}
		
	return false;
}
