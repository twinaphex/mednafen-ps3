#include <es_system.h>

					WiiInput::WiiInput						()
{
}

					WiiInput::~WiiInput						()
{
}

uint32_t			WiiInput::PadCount						()
{
	return 1;
}

void				WiiInput::Reset							()
{
	WPAD_ScanPads();
}

void				WiiInput::Refresh						()
{
	WPAD_ScanPads();
}

int32_t				WiiInput::GetAxis						(uint32_t aPad, uint32_t aAxis)
{
	//TODO: 
	return 0;
}

bool				WiiInput::ButtonPressed					(uint32_t aPad, uint32_t aButton)
{
	WPAD_ScanPads(); //:HACK
	return WPAD_ButtonsHeld(aPad) & (1 << aButton);
}

bool				WiiInput::ButtonDown					(uint32_t aPad, uint32_t aButton)
{
	WPAD_ScanPads(); //:HACK
	return WPAD_ButtonsDown(aPad) & (1 << aButton);
}

uint32_t			WiiInput::GetAnyButton					(uint32_t aPad)
{
	WPAD_ScanPads(); //:HACK
	for(int i = 0; i != 32; i ++)
	{
		if(WPAD_ButtonsDown(aPad) & (1 << i))
		{
			return i;
		}
	}

	return 0xFFFFFFFF;
}

std::string			WiiInput::GetButtonName					(uint32_t aButton)
{
	char buffer[256];
	snprintf(buffer, 256, "BUTTON%d", aButton);
	return buffer;
}

std::string			WiiInput::GetButtonImage				(uint32_t aButton)
{
	return "NOIMAGE";
}
