#include <es_system.h>
#include "SummerfaceLabel.h"

											SummerfaceLabel::SummerfaceLabel					(const Area& aRegion, const std::string& aMessage) : SummerfaceWindow(aRegion), Wrap(false)
{
	SetMessage(aMessage);
}

											SummerfaceLabel::SummerfaceLabel					(const Area& aRegion, const char* aFormat, ...) : SummerfaceWindow(aRegion), Wrap(false)
{
	char array[2048];

	va_list args;
	va_start (args, aFormat);
	vsnprintf(array, 2048, aFormat, args);
	va_end(args);

	SetMessage(array);
}

											SummerfaceLabel::~SummerfaceLabel					()
{

}

bool										SummerfaceLabel::Draw								()
{
	//If it fits, just print
	if(!Wrap || FontManager::GetBigFont()->MeasureString(GetMessage().c_str()) < ESVideo::GetClip().Width)
	{
		FontManager::GetBigFont()->PutString(GetMessage().c_str(), 2, 2, Colors::Normal, true);	
	}
	else
	{
		//Some state variables
		size_t position = 0;
		const std::string message = GetMessage();
		std::stringstream line;
		int online = 0;

		//Scan the message
		do
		{
			//Find the next word
			size_t wordpos = message.find_first_of(' ', position);
			std::string word = message.substr(position, wordpos - position);

			//If this word would put the line over the size, print and goto the next line
			if(FontManager::GetBigFont()->MeasureString((line.str() + word).c_str()) > ESVideo::GetClip().Width)
			{
				//Handle the case where one word would excede the total length
				if(line.str().empty())
				{
					FontManager::GetBigFont()->PutString(word.c_str(), 2, 2 + FontManager::GetBigFont()->GetHeight() * online, Colors::Normal, true);
					word = "";
				}
				else
				{
					FontManager::GetBigFont()->PutString(line.str().c_str(), 2, 2 + FontManager::GetBigFont()->GetHeight() * online, Colors::Normal, true);
					line.str("");
				}
				online ++;
			}

			//Add the word to the line cache
			if(!word.empty())
			{
				line << word << " ";
			}

			//Get the position of the next word
			position = (wordpos == std::string::npos) ? wordpos : wordpos + 1;
		}	while(position != std::string::npos);

		//Print the last line if needed
		if(!line.str().empty())
		{
			FontManager::GetBigFont()->PutString(line.str().c_str(), 2, 2 + FontManager::GetBigFont()->GetHeight() * online, Colors::Normal, true);				
		}
	}

	return false;
}

std::string									SummerfaceLabel::GetMessage							()
{
	return Message;
}

void										SummerfaceLabel::SetMessage							(const char* aFormat, ...)
{
	char array[2048];

	va_list args;
	va_start (args, aFormat);
	vsnprintf(array, 2048, aFormat, args);
	va_end(args);

	Message = array;
}


void										SummerfaceLabel::SetMessage							(const std::string& aMessage)
{
	Message = aMessage;
}

void										SummerfaceLabel::AppendMessage						(const std::string& aMessage)
{
	SetMessage("%s%s", GetMessage().c_str(), aMessage.c_str());
}

