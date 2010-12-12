#include <ps3_system.h>

						TextViewer::TextViewer					(std::string aFileName) : Winterface(aFileName)
{
	std::ifstream file(aFileName.c_str());
	
	if(file.fail())
	{
		Lines.push_back("Couldn't open file");
		LongestLine = Lines[0].length();
		Top = Left = LinesDrawn = 0;
		return;
	}

	LoadStream(file);	
	
	file.close();
}
						
						TextViewer::TextViewer					(std::string aText, std::string aHeader) : Winterface(aHeader)
{
	std::stringstream text(aText);
	LoadStream(text);
}
								
						TextViewer::~TextViewer					()
{
						
}
		
bool					TextViewer::DrawLeft					()
{
	uint32_t lineheight = FontManager::GetFixedFont()->GetHeight();
	LinesDrawn = PS3Video::GetClip().Height / lineheight;
	
	for(int i = 0; i != LinesDrawn; i ++)
	{
		if(i + Top >= Lines.size())
		{
			break;
		}
		
		if(Lines[Top + i].length() > Left)
		{
			FontManager::GetFixedFont()->PutString(Lines[Top + i].substr(Left), 2, i * lineheight, Colors::Normal);
		}
	}
	
	return false;	
}

bool					TextViewer::Input						()
{
	Top += PS3Input::ButtonPressed(0, PS3_BUTTON_DOWN) ? 1 : 0;
	Top -= PS3Input::ButtonPressed(0, PS3_BUTTON_UP) ? 1 : 0;
	Top += PS3Input::ButtonPressed(0, PS3_BUTTON_R1) ? LinesDrawn : 0;
	Top -= PS3Input::ButtonPressed(0, PS3_BUTTON_L1) ? LinesDrawn : 0;
	Top = Utility::Clamp(Top, 0, (int32_t)Lines.size() - 1);
	
	Left += PS3Input::ButtonPressed(0, PS3_BUTTON_RIGHT) ? 1 : 0;
	Left -= PS3Input::ButtonPressed(0, PS3_BUTTON_LEFT) ? 1 : 0;	
	Left = Utility::Clamp(Left, 0, (int32_t)LongestLine);

	return PS3Input::ButtonDown(0, PS3_BUTTON_CROSS);
}
		
void					TextViewer::LoadStream					(std::istream& aStream)
{
	std::string line;

	SetInputDelay(2);

	Top = 0;
	Left = 0;
	LongestLine = 0;
	LinesDrawn = 0;
		
	while(!aStream.eof())
	{
		std::getline(aStream, line);
		Lines.push_back(line);
		
		LongestLine = (line.length() > LongestLine) ? line.length() : LongestLine;
	}
	
	SideItems.push_back(new ListItem("[DPAD] Navigate", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[X] Close", FontManager::GetSmallFont()));	
	SideItems.push_back(new ListItem("[L1] Page Up", FontManager::GetSmallFont()));	
	SideItems.push_back(new ListItem("[R1] Page Down", FontManager::GetSmallFont()));
}