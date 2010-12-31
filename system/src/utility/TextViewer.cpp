#include <ps3_system.h>

						TextViewer::TextViewer					(const std::string& aFileName) : Winterface(aFileName)
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
						
						TextViewer::TextViewer					(const std::string& aText, const std::string& aHeader) : Winterface(aHeader)
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
	LinesDrawn = es_video->GetClip().Height / lineheight;
	
	for(int i = 0; i != LinesDrawn; i ++)
	{
		if(i + Top >= Lines.size())
		{
			break;
		}
		
		if(Lines[Top + i].length() > Left)
		{
			FontManager::GetFixedFont()->PutString(Lines[Top + i].substr(Left).c_str(), 2, i * lineheight, Colors::Normal);
		}
	}
	
	return false;	
}

bool					TextViewer::Input						()
{
	Top += es_input->ButtonPressed(0, ES_BUTTON_DOWN) ? 1 : 0;
	Top -= es_input->ButtonPressed(0, ES_BUTTON_UP) ? 1 : 0;
	Top += es_input->ButtonPressed(0, ES_BUTTON_AUXRIGHT1) ? LinesDrawn : 0;
	Top -= es_input->ButtonPressed(0, ES_BUTTON_AUXLEFT1) ? LinesDrawn : 0;
	Top = Utility::Clamp(Top, 0, (int32_t)Lines.size() - 1);
	
	Left += es_input->ButtonPressed(0, ES_BUTTON_RIGHT) ? 1 : 0;
	Left -= es_input->ButtonPressed(0, ES_BUTTON_LEFT) ? 1 : 0;	
	Left = Utility::Clamp(Left, 0, (int32_t)LongestLine);

	return es_input->ButtonDown(0, ES_BUTTON_ACCEPT);
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

