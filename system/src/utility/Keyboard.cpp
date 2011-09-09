#include <es_system.h>
#include "Keyboard.h"

namespace
{
	const char*		Chars[2][5] = {{"`1234567890-=", "qwertyuiop[]\\", "asdfghjkl;'", "zxcvbnm,./", " "}, {"!@#$%^&*()_+", "QWERTYUIOP{}|", "ASDFGHJKL:\"", "ZXCVBNM<>?", " "}};
}

							Keyboard::Keyboard								(const Area& aRegion, const std::string& aHeader, const std::string& aText) :
	SummerfaceWindow(aRegion),
	TextColor("text", Colors::black),
	SelectedColor("selectedtext", Colors::red)
{
	Text.reserve(256);

	Text = aText;
	SetHeader(aHeader);

	Column = 0;
	Row = 0;
	Shift = 0;
	Canceled = false;
}

bool						Keyboard::Draw									()
{
	FontManager::GetBigFont()->PutString(Text.c_str(), 8, FontManager::GetBigFont()->GetHeight() + 2, TextColor);
	
	uint32_t startX = 0;
	
	for(int i = 0; i != 4; i ++)
	{
		for(int j = 0; j != strlen(Chars[Shift][i]); j ++)
		{
			char charr[2] = {Chars[Shift][i][j], 0};
			FontManager::GetBigFont()->PutString(charr, startX + (j * 2) * FontManager::GetBigFont()->GetWidth(), (i + 4) * FontManager::GetBigFont()->GetHeight(), (i == Row && j == Column) ? SelectedColor : TextColor);
		}
	}

	FontManager::GetBigFont()->PutString("     [SPACE]", startX, (4 + 4) * FontManager::GetBigFont()->GetHeight(), (Row == 4) ? SelectedColor : TextColor);			

	return false;	
}

bool						Keyboard::Input									(uint32_t aButton)
{
	Row += (aButton == ES_BUTTON_DOWN) ? 1 : 0;
	Row -= (aButton == ES_BUTTON_UP) ? 1 : 0;
	Row = Utility::Clamp(Row, 0, 4);
	
	Column += (aButton == ES_BUTTON_RIGHT) ? 1 : 0;
	Column -= (aButton == ES_BUTTON_LEFT) ? 1 : 0;
	Column = Utility::Clamp(Column, 0, strlen(Chars[0][Row]) - 1);

	if(aButton == ES_BUTTON_ACCEPT)
	{
		Text.push_back(Chars[Shift][Row][Column]);
	}
	else if(aButton == ES_BUTTON_SHIFT)
	{
		Shift = Shift == 0 ? 1 : 0;
	}
	else if(aButton == ES_BUTTON_TAB)
	{
		if(!Text.empty())
		{
			Text.erase(Text.length() - 1);
		}
	}
	else if(aButton == ES_BUTTON_CANCEL)
	{
		return true;
	}
		
	return false;
}

