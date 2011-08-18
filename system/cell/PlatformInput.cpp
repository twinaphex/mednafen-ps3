#include <es_system.h>
#include <cell/pad.h>

namespace
{
	const char*					vaprint									(const char* aFormat, ...)
	{
		static char buf[1024];
		va_list args;
		va_start(args, aFormat);
		vsnprintf(buf, 1024, aFormat, args);
		va_end(args);

		return buf;
	}

	//Pad data	
	const uint32_t				esIndex[14] = {4 + 8, 6 + 8, 7 + 8, 5 + 8, 14 + 8, 13 + 8, 12 + 8, 15 + 8, 10 + 8, 11 + 8, 8 + 8, 9 + 8, 1 + 8, 2 + 8};
	const char* const			AxisNames[] = {"Left X", "Left Y", "Right X", "Right Y"};
	const char* const			ButtonNames[] = {"Select", "L3", "R3", "Start", "Up", "Right", "Down", "Left", "L2", "R2", "L1", "R1", "Triangle", "Circle", "Cross", "Square"};

	//libpad data
	CellPadInfo2				PadInfo;
	CellPadData					CurrentState[4];

	//Callbacks to obtain button state.
	bool						FetchAxisLow							(uint32_t aPad, uint32_t aAxis, uint32_t aA)
	{
		int realaxis = 4 + (4 - 1 - aAxis);
		return CurrentState[aPad].button[realaxis] < 0x40;
	}

	bool						FetchAxisHigh							(uint32_t aPad, uint32_t aAxis, uint32_t aA)
	{
		int realaxis = 4 + (4 - 1 - aAxis);
		return CurrentState[aPad].button[realaxis] > 0xC0;
	}

	bool						FetchButton								(uint32_t aPad, uint32_t aButton, uint32_t aA)
	{
		uint32_t totalButtons = CurrentState[aPad].button[2] | (CurrentState[aPad].button[3] << 8);
		return (totalButtons & (1 << aButton)) ? true : false;
	}
}

void							ESInputPlatform::Initialize				(ESInput::InputDeviceList& aDevices, ESInput::InputDeviceList& aSubDevices, uint32_t aESKeyIndex[14])
{
	//Copy the system key map
	memcpy(aESKeyIndex, esIndex, sizeof(esIndex));

	//Setup libpad
	cellPadInit(4);
	cellPadGetInfo2(&PadInfo);

	for(int i = 0; i != 4; i ++)
	{
		aDevices.push_back(ESInput::InputDevice());

		//Axis
		for(int j = 0; j != 4; j ++)
		{
			aDevices[i].push_back(ESInput::Button(FetchAxisLow, i, j, 0, vaprint("Pad %d %s Axis Low", i, AxisNames[j])));
			aDevices[i].push_back(ESInput::Button(FetchAxisHigh, i, j, 0, vaprint("Pad %d %s Axis High", i, AxisNames[j])));
		}

		//Buttons
		for(int j = 0; j != 16; j ++)
		{
			aDevices[i].push_back(ESInput::Button(FetchButton, i, j, 0, vaprint("Pad %d %s", i, ButtonNames[j])));
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


