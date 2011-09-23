#include <es_system.h>
#include "Keyboard.h"

namespace
{
	const char*		Chars[2][5] = {{"`1234567890-=", "qwertyuiop[]\\", "asdfghjkl;'", "zxcvbnm,./", " "}, {"!@#$%^&*()_+", "QWERTYUIOP{}|", "ASDFGHJKL:\"", "ZXCVBNM<>?", " "}};
	const uint32_t	TotalRows = 5;
	const uint32_t	RowLengths[TotalRows] = {13, 13, 11, 10, 1};
}

							Keyboard::Keyboard								(const Area& aRegion, const std::string& aHeader, const std::string& aText) :
	SummerfaceWindow(aRegion),
	Text(aText),
	Column(0, ES_BUTTON_LEFT, ES_BUTTON_RIGHT),
	Row(0, ES_BUTTON_UP, ES_BUTTON_DOWN),
	Shift(0),
	TextColor("text", Colors::black),
	SelectedColor("selectedtext", Colors::red)
{
	SetHeader(aHeader);
}

bool						Keyboard::Draw									()
{
	Font* font = FontManager::GetBigFont();

	font->PutString(Text.c_str(), 8, FontManager::GetBigFont()->GetHeight() + 2, TextColor);
	
	for(int i = 0; i != TotalRows; i ++)
	{
		for(int j = 0; j != RowLengths[i]; j ++)
		{
			font->PutString(&Chars[Shift ? 1 : 0][i][j], 1, j * font->GetWidth(), (i + 4) * font->GetHeight(), (i == Row && j == Column) ? SelectedColor : TextColor);
		}
	}

	font->PutString("     [SPACE]", 0, 8 * font->GetHeight(), (Row == 4) ? SelectedColor : TextColor);

	return false;
}

bool						Keyboard::Input									(uint32_t aButton)
{
	Row.Scroll(aButton, TotalRows);
	Column.Scroll(aButton, RowLengths[Row]);
	Shift = (aButton == ES_BUTTON_SHIFT) ? !Shift : Shift;

	if(aButton == ES_BUTTON_ACCEPT)
	{
		Text.push_back(Chars[Shift ? 1 : 0][Row][Column]);
	}
	else if(aButton == ES_BUTTON_TAB && !Text.empty())
	{
		Text.erase(Text.length() - 1);
	}

	return (aButton == ES_BUTTON_CANCEL);
}

