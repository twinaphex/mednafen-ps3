#include <es_system.h>

void							ESInputPlatform::Initialize				(ESInput::InputDeviceList& aDevices, ESInput::InputDeviceList& aSubDevices, uint32_t aESKeyIndex[14])
{
	cellPadInit(4);
	cellPadGetInfo2(&PadInfo);

	for(int i = 0; i != 4; i ++)
	{
		aDevices.push_back(ESInput::InputDevice());

		//Axis
		for(int j = 0; j != 4; j ++)
		{
			aDevices[i].push_back(ESInput::Button(FetchAxisLow, i, j, 0, ""));
			aDevices[i].push_back(ESInput::Button(FetchAxisHigh, i, j, 0, ""));
		}

		//Buttons
		for(int j = 0; j != 16; j ++)
		{
			aDevices[i].push_back(ESInput::Button(FetchButton, i, j, 0, ""));
		}
	}
}

void							ESInputPlatform::Shutdown				()
{
	cellPadEnd();
}

void							ESInputPlatform::Refresh				()
{
	cellPadGetInfo2(&PadInfo);

	if(!(PadInfo.system_info & CELL_PAD_INFO_INTERCEPTED))
	{
		for(int p = 0; p != PadInfo.now_connect && p != 4; p ++)
		{
			CellPadData newstate;
			cellPadGetData(p, &newstate);

			if(newstate.len != 0)
			{
				CurrentState[p] = newstate;
			}
		}
	}
}

bool							ESInputPlatform::FetchAxisLow			(uint32_t aPad, uint32_t aAxis, uint32_t aA)
{
	int realaxis = 4 + (4 - 1 - aAxis);
	return CurrentState[aPad].button[realaxis] < 0x40;
}

bool							ESInputPlatform::FetchAxisHigh			(uint32_t aPad, uint32_t aAxis, uint32_t aA)
{
	int realaxis = 4 + (4 - 1 - aAxis);
	return CurrentState[aPad].button[realaxis] > 0xC0;
}

bool							ESInputPlatform::FetchButton			(uint32_t aPad, uint32_t aButton, uint32_t aA)
{
	uint32_t totalButtons = CurrentState[aPad].button[0] | (CurrentState[aPad].button[1] << 8);
	return totalButtons & (1 << aButton);
}

CellPadInfo2					ESInputPlatform::PadInfo;
CellPadData						ESInputPlatform::CurrentState[4];

