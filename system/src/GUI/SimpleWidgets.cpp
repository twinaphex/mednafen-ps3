#include <es_system.h>

								SummerfaceLabel::SummerfaceLabel				(const Area& aRegion, const std::string& aMessage) :
	SummerfaceWindow(aRegion),
	Wrap(false)
{
	SetMessage(aMessage);
}

								SummerfaceLabel::SummerfaceLabel				(const Area& aRegion, const char* aFormat, ...) :
	SummerfaceWindow(aRegion),
	Wrap(false)
{
	char array[2048];

	va_list args;
	va_start (args, aFormat);
	vsnprintf(array, 2048, aFormat, args);
	va_end(args);

	SetMessage(array);
}

bool							SummerfaceLabel::Draw							()
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

void							SummerfaceLabel::SetMessage						(const char* aFormat, ...)
{
	char array[2048];

	va_list args;
	va_start (args, aFormat);
	vsnprintf(array, 2048, aFormat, args);
	va_end(args);

	Message = array;
}

/////////////////
//SummerfaceImage
/////////////////
								SummerfaceImage::SummerfaceImage				(const Area& aRegion, const std::string& aAsset) :
	SummerfaceWindow(aRegion),
	Image(aAsset)
{

}

bool							SummerfaceImage::Draw							()
{
	if(ImageManager::GetImage(Image))
	{
		Texture* tex = ImageManager::GetImage(Image);

		uint32_t x = 1, y = 1, w = ESVideo::GetClip().Width - 2, h = ESVideo::GetClip().Height - 2;
		Utility::CenterAndScale(x, y, w, h, tex->GetWidth(), tex->GetHeight());

		ESVideo::PlaceTexture(tex, Area(x, y, w, h), Area(0, 0, tex->GetWidth(), tex->GetHeight()), 0xFFFFFFFF);
	}

	return false;
}

//////////////////
//SummerfaceNumber
//////////////////
								SummerfaceNumber::SummerfaceNumber				(const Area& aRegion, int64_t aValue, uint32_t aDigits, bool aHex) :
	SummerfaceWindow(aRegion),
	SelectedIndex(31),
	Digits(aDigits),
	Hex(aHex)
{
	SetValue(aValue);
}


bool							SummerfaceNumber::Draw							()
{
	for(int i = i; i != Digits; i ++)
	{
		std::string chara = std::string(1, Value[(32 - Digits) + i]);
		FontManager::GetFixedFont()->PutString(chara.c_str(), 2 + i * FontManager::GetFixedFont()->GetWidth(), 2, ((32 - Digits) + i == SelectedIndex) ? Colors::HighLight : Colors::Normal, true);
	}

	return false;
}

bool							SummerfaceNumber::Input							(uint32_t aButton)
{
	SelectedIndex += (aButton == ES_BUTTON_LEFT) ? -1 : 0;
	SelectedIndex += (aButton == ES_BUTTON_RIGHT) ? 1 : 0;
	SelectedIndex = Utility::Clamp(SelectedIndex, 32 - Digits, 31);

	if(aButton == ES_BUTTON_UP) IncPosition(SelectedIndex);
	if(aButton == ES_BUTTON_DOWN) DecPosition(SelectedIndex);

	Canceled = (aButton == ES_BUTTON_CANCEL);
	return aButton == ES_BUTTON_ACCEPT || aButton == ES_BUTTON_CANCEL;
}

int64_t							SummerfaceNumber::GetValue						()
{
	//Strip leading zeroes
	const char* pvalue = Value;
	while(*pvalue != 0 && *pvalue == '0')
	{
		pvalue ++;
	}

	//Get value based on mode
	int64_t value;
	sscanf(pvalue, Hex ? "%llX" : "%lld", (long long int*)&value);

	return value;
}

void							SummerfaceNumber::SetValue						(int64_t aValue)
{
	//Print base on mode
	snprintf(Value, 33, Hex ? "%032llX" : "%032lld", (long long int)aValue);
}

void							SummerfaceNumber::IncPosition					(uint32_t aPosition)
{
	if(aPosition >= (32 - Digits) && aPosition < 32)
	{
		Value[aPosition] ++;

		//Going past 9
		if(Value[aPosition] == '9' + 1)
		{
			//Goto proper value
			Value[aPosition] = Hex ? 'A' : '0';
		}

		//Going past F in hex
		if(Hex && Value[aPosition] == 'F' + 1)
		{
			Value[aPosition] = '0';
		}
	}
}

void							SummerfaceNumber::DecPosition					(uint32_t aPosition)
{
	if(aPosition >= (32 - Digits) && aPosition < 32)
	{
		Value[aPosition] --;

		//Going below 0
		if(Value[aPosition] == '0' - 1)
		{
			//Goto appropriate value base on mode
			Value[aPosition] = Hex ? 'F' : '9';
		}

		//Going below A in hex
		if(Hex && Value[aPosition] == 'A' - 1)
		{
			Value[aPosition] = '9';
		}
	}
}

