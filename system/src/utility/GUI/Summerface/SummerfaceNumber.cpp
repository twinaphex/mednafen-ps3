#include <es_system.h>
#include "SummerfaceNumber.h"

bool										SummerfaceNumber::Draw								()
{
	for(int i = i; i != Digits; i ++)
	{
		std::string chara = std::string(1, Value[(32 - Digits) + i]);
		FontManager::GetFixedFont()->PutString(chara.c_str(), 2 + i * FontManager::GetFixedFont()->GetWidth(), 2, ((32 - Digits) + i == SelectedIndex) ? Colors::HighLight : Colors::Normal, true);
	}

	return false;
}

bool										SummerfaceNumber::Input								()
{
	SelectedIndex += ESInput::ButtonPressed(0, ES_BUTTON_LEFT) ? -1 : 0;
	SelectedIndex += ESInput::ButtonPressed(0, ES_BUTTON_RIGHT) ? 1 : 0;
	SelectedIndex = Utility::Clamp(SelectedIndex, 32 - Digits, 31);

	if(ESInput::ButtonPressed(0, ES_BUTTON_UP)) IncPosition(SelectedIndex);
	if(ESInput::ButtonPressed(0, ES_BUTTON_DOWN)) DecPosition(SelectedIndex);

	Canceled = ESInput::ButtonPressed(0, ES_BUTTON_CANCEL);
	return ESInput::ButtonDown(0, ES_BUTTON_ACCEPT) || ESInput::ButtonDown(0, ES_BUTTON_CANCEL);
}

int64_t										SummerfaceNumber::GetValue							()
{
	const char* pvalue = Value;
	while(*pvalue != 0 && *pvalue == '0')
	{
		pvalue ++;
	}

	return atoi(pvalue);
}

void										SummerfaceNumber::SetValue							(int64_t aValue)
{
	snprintf(Value, 33, "%032lld", aValue);
}

void										SummerfaceNumber::IncPosition						(uint32_t aPosition)
{
	if(aPosition < 32)
	{
		Value[aPosition] ++;

		if(Value[aPosition] == '9' + 1)
		{
			Value[aPosition] = '0';
			IncPosition(aPosition - 1);
		}
	}
}

void										SummerfaceNumber::DecPosition						(uint32_t aPosition)
{
	if(aPosition < 32)
	{
		Value[aPosition] --;

		if(Value[aPosition] == '0' - 1)
		{
			Value[aPosition] = '9';
			DecPosition(aPosition - 1);
		}
	}
}


