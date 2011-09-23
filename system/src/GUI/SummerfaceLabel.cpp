#include <es_system.h>

								SummerfaceLabel::SummerfaceLabel				(const Area& aRegion, const std::string& aFormat, ...) :
	SummerfaceWindow(aRegion),
	Wrap(false),
	TextColor("text", Colors::black)
{
	char array[2048];

	va_list args;
	va_start (args, aFormat);
	vsnprintf(array, 2048, aFormat.c_str(), args);
	va_end(args);

	SetMessage(array);
}

bool							SummerfaceLabel::Draw							()
{
	const std::string message = GetMessage();
	Font* font = FontManager::GetBigFont();

	//If it fits, just print
	if(!Wrap || font->MeasureString(message.c_str()) < ESVideo::GetClip().Width)
	{
		font->PutString(message.c_str(), 0, 0, TextColor, true);	
	}
	else
	{
		//Some state variables
		size_t position = 0;
		std::stringstream line;
		int online = 0;

		//Scan the message
		do
		{
			//Find the next word
			size_t wordpos = message.find_first_of(' ', position);
			std::string word = message.substr(position, wordpos - position);

			//If this word would put the line over the size, print and goto the next line
			if(font->MeasureString((line.str() + word).c_str()) > ESVideo::GetClip().Width)
			{
				//Handle the case where one word would excede the total length
				if(line.str().empty())
				{
					font->PutString(word.c_str(), 0, font->GetHeight() * online, TextColor, true);
					word = "";
				}
				else
				{
					font->PutString(line.str().c_str(), 0, font->GetHeight() * online, TextColor, true);
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
			font->PutString(line.str().c_str(), 0, font->GetHeight() * online, TextColor, true);				
		}
	}

	return false;
}

void							SummerfaceLabel::SetMessage						(const std::string& aFormat, ...)
{
	char array[2048];

	va_list args;
	va_start (args, aFormat);
	vsnprintf(array, 2048, aFormat.c_str(), args);
	va_end(args);

	Message = array;
}


