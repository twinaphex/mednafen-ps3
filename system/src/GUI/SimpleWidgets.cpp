#include <es_system.h>


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
	Texture* tex = ImageManager::GetImage(Image);

	if(ImageManager::GetImage(Image))
	{
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
	SelectedIndex(0, ES_BUTTON_RIGHT, ES_BUTTON_LEFT),
	Digits(aDigits),
	Hex(aHex),
	TextColor("text", Colors::black),
	SelectedColor("selectedtext", Colors::red)
{
	SetValue(aValue);
}


bool							SummerfaceNumber::Draw							()
{
	for(int i = 0; i != Digits; i ++)
	{
		FontManager::GetFixedFont()->PutString(&Value[(32 - Digits) + i], 1, i * FontManager::GetFixedFont()->GetWidth(), 0, ((32 - Digits + i) == (31 - SelectedIndex)) ? SelectedColor : TextColor, true);
	}

	return false;
}

bool							SummerfaceNumber::Input							(uint32_t aButton)
{
	SelectedIndex.Scroll(aButton, Digits);

	if(aButton == ES_BUTTON_UP) IncPosition(31 - SelectedIndex);
	if(aButton == ES_BUTTON_DOWN) DecPosition(31 - SelectedIndex);

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

//////////////////
//SummerfaceButton
//////////////////
								SummerfaceButton::SummerfaceButton				(const Area& aRegion, const std::string& aButtonName) :
		SummerfaceLabel(aRegion, "Press button for "),
		LastButton(0xFFFFFFFF),
		ButtonID(0xFFFFFFFF)
	{
		AppendMessage(aButtonName);
	}

bool							SummerfaceButton::Input							(uint32_t aButton)
{
	//Don't continue until all buttons are released
	if(ButtonID != 0xFFFFFFFF && ESInput::GetAnyButton() != 0xFFFFFFFF)
	{
		return false;
	}

	//Get a button from the input engine and store it
	ButtonID = ESInput::GetAnyButton();

	//Note wheather a button has beed pressed for next call
	return ButtonID != 0xFFFFFFFF;
}

