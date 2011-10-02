#include <es_system.h>

void							ESInput_Button::SetState		(uint16_t aPressed)
{
	//Update jammed state
	Jammed = (Jammed && (aPressed > 0x2000));

	//Only if the state is changed
	if(((Pressed < 0x2000) && (aPressed > 0x2000)) || ((Pressed > 0x2000) && (aPressed < 0x2000)))
	{
		Inspected = false;
		PressedTime = Utility::GetTicks();
	}

	Pressed = aPressed;
}

uint16_t						ESInput_Button::GetStateRepeat	()
{
	if(GetStateInspected())
	{
		PressedTime = Utility::GetTicks();
		return Pressed;
	}
	else if(GetState())
	{
		uint32_t time = Utility::GetTicks();
		if((time - PressedTime) > 250)
		{
			PressedTime += 250;
			return Pressed;
		}
	}

	return 0;
}

void							ESInput::Initialize				()
{
	ESInputPlatform::Initialize(Inputs, SubInputs, ESKeyIndex);
}

void							ESInput::Shutdown				()
{
	ESInputPlatform::Shutdown();
	Inputs.clear();
}

uint32_t						ESInput::WaitForESKey			(bool aGuarantee)
{
	do
	{
		//Scan the input
		Refresh();

		//Look for a press
		for(uint32_t i = 0; i != 14; i ++)
		{
			ESInput_Button* button = GetButton(ES_BUTTON_UP + i);
			if(button && (button->GetStateRepeat() > 0x8000))
			{
				return ES_BUTTON_UP + i;
			}
		}
	}	while(!LibES::WantToDie() && !LibES::WantToSleep() && aGuarantee);

	//Give a default
	return 0xFFFFFFFF;
}

void							ESInput::Reset					()
{
	//Reset main devices
	for(InputDeviceList::iterator i = Inputs.begin(); i != Inputs.end(); i ++)
	{
		for(InputDevice::iterator j = i->begin(); j != i->end(); j ++)
		{
			j->Reset();
		}
	}

	//Reset sub devices
	for(InputDeviceList::iterator i = SubInputs.begin(); i != SubInputs.end(); i ++)
	{
		for(InputDevice::iterator j = i->begin(); j != i->end(); j ++)
		{
			j->Reset();
		}
	}
}

void							ESInput::Refresh				()
{
	//Platform refresh
	ESInputPlatform::Refresh();

	//Refresh main devices
	for(InputDeviceList::iterator i = Inputs.begin(); i != Inputs.end(); i ++)
	{
		for(InputDevice::iterator j = i->begin(); j != i->end(); j ++)
		{
			j->SetState(j->Refresh ? j->Refresh(j->User1, j->User2, j->User3) : 0);
		}
	}

	//Refresh sub devices
	for(InputDeviceList::iterator i = SubInputs.begin(); i != SubInputs.end(); i ++)
	{
		for(InputDevice::iterator j = i->begin(); j != i->end(); j ++)
		{
			j->SetState(j->Refresh ? j->Refresh(j->User1, j->User2, j->User3) : 0);
		}
	}
}

uint32_t						ESInput::GetAnyButton			()
{
	Refresh();

	//Check main devices
	for(int i = 0; i != Inputs.size(); i ++)
	{
		for(int j = 0; j != Inputs[i].size(); j ++)
		{
			if(Inputs[i][j].GetState() > 0x8000)
			{
				return (i * 1024) + j;
			}
		}
	}

	//Check sub devices
	for(int i = 0; i != SubInputs.size(); i ++)
	{
		for(int j = 0; j != SubInputs[i].size(); j ++)
		{
			if(SubInputs[i][j].GetState() > 0x8000)
			{
				return 0x10000 + (0x10000 * i) + j;
			}
		}
	}

	return 0xFFFFFFFF;
}


ESInput::InputDeviceList		ESInput::Inputs;
ESInput::InputDeviceList		ESInput::SubInputs;
uint32_t						ESInput::ESKeyIndex[14];

