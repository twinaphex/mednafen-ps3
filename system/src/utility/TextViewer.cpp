#include <es_system.h>
#include "TextViewer.h"

						TextViewer::TextViewer					(const Area& aRegion, const char* aFileName, bool aFile) :
	SummerfaceWindow(aRegion),
	Top(0, ES_BUTTON_UP, ES_BUTTON_DOWN, ES_BUTTON_AUXLEFT1, ES_BUTTON_AUXRIGHT1, ES_BUTTON_AUXLEFT2, ES_BUTTON_AUXRIGHT2),
	Left(0, ES_BUTTON_LEFT, ES_BUTTON_RIGHT),
	LongestLine(0),
	LinesDrawn(0),
	TextColor("text", Colors::black)
{
	SetHeader("[Text Viewer]");
	Reload(aFileName, aFile);
}

void					TextViewer::Clear						()
{
	Lines.clear();

	Top = 0;
	Left = 0;
	LongestLine = 0;
	LinesDrawn = 0;
}

void					TextViewer::Reload						(const char* aFileName, bool aFile)
{
	assert(aFileName);

	std::istream* stream = aFile ? (std::istream*) new std::ifstream(aFileName) : (std::istream*) new std::stringstream(aFileName);

	if(!stream->fail())
	{
		LoadStream(stream);	
	}
	else
	{
		Clear();
		Lines.push_back("Couldn't open file");
		LongestLine = Lines[0].length();
		return;
	}

	delete stream;
}

void					TextViewer::AppendLine					(const char* aMessage, ...)
{
	char array[1024];
	va_list args;
	va_start (args, aMessage);
	vsnprintf(array, 1024, aMessage, args);
	va_end(args);

	Lines.push_back(std::string(array));

	LongestLine = Lines[Lines.size() - 1].length() > LongestLine ? Lines[Lines.size() - 1].length() : LongestLine;
}

bool					TextViewer::Draw						()
{
	uint32_t lineheight = FontManager::GetFixedFont()->GetHeight();
	LinesDrawn = ESVideo::GetClip().Height / lineheight;
	
	for(int i = 0; (i != LinesDrawn) && ((i + Top) < Lines.size()); i ++)
	{
		if(Lines[Top + i].length() > Left)
		{
			FontManager::GetFixedFont()->PutString(Lines[Top + i].substr(Left).c_str(), 2, i * lineheight, TextColor);
		}
	}
	
	return false;	
}

bool					TextViewer::Input						(uint32_t aButton)
{
	Top.Scroll(aButton, Lines.size() -1, LinesDrawn);
	Left.Scroll(aButton, LongestLine);
	return (aButton == ES_BUTTON_ACCEPT) || (aButton == ES_BUTTON_CANCEL);
}
		
void					TextViewer::LoadStream					(std::istream* aStream)
{
	assert(aStream && !aStream->fail());

	Clear();

	std::string line;
	while(!aStream->eof())
	{
		std::getline(*aStream, line);
		Lines.push_back(line);
		
		LongestLine = (line.length() > LongestLine) ? line.length() : LongestLine;
	}
}

