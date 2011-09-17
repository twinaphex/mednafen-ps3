#include <es_system.h>

void							ESInput::Button::SetState		(bool aPressed)
{
	//Update jammed state
	Jammed = (Jammed && aPressed);

	//Only if the state is changed
	if(Pressed != aPressed)
	{
		Pressed = aPressed;
		Inspected = false;
		PressedTime = Utility::GetTicks();
	}
}

bool							ESInput::Button::GetStateRepeat	()
{
	if(GetStateInspected())
	{
		PressedTime = Utility::GetTicks();
		return true;
	}
	else if(GetState())
	{
		uint32_t time = Utility::GetTicks();
		if((time - PressedTime) > 250)
		{
			PressedTime += 250;
			return true;
		}
	}

	return false;
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
			Button* button = GetButton(ES_BUTTON_UP + i);
			if(button && button->GetStateRepeat())
			{
				return ES_BUTTON_UP + i;
			}
		}
	}	while(!WantToDie() && !WantToSleep() && aGuarantee);

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
			j->SetState(j->Refresh ? j->Refresh(j->User1, j->User2, j->User3) : false);
		}
	}

	//Refresh sub devices
	for(InputDeviceList::iterator i = SubInputs.begin(); i != SubInputs.end(); i ++)
	{
		for(InputDevice::iterator j = i->begin(); j != i->end(); j ++)
		{
			j->SetState(j->Refresh ? j->Refresh(j->User1, j->User2, j->User3) : false);
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
			if(Inputs[i][j].GetState())
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
			if(SubInputs[i][j].GetState())
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

