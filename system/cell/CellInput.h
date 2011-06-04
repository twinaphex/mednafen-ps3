#pragma once

enum
{
	ES_BUTTON_UP = 6, ES_BUTTON_DOWN = 7, ES_BUTTON_LEFT = 8, ES_BUTTON_RIGHT = 9, ES_BUTTON_ACCEPT = 3, ES_BUTTON_CANCEL = 5,
	ES_BUTTON_SHIFT = 2, ES_BUTTON_TAB = 4, ES_BUTTON_AUXLEFT1 = 10, ES_BUTTON_AUXRIGHT1 = 13, ES_BUTTON_AUXLEFT2 = 11, ES_BUTTON_AUXRIGHT2 = 14, 
	ES_BUTTON_AUXLEFT3 = 12, ES_BUTTON_AUXRIGHT3 = 15 
};

class								ESInput
{
	public:
		static void					Initialize				();
		static void					Shutdown				();
					
		static uint32_t				PadCount				();
		static void					Reset					();
		static void					Refresh					();
		
		static int32_t				GetAxis					(uint32_t aPad, uint32_t aAxis);

		static bool					ButtonPressed			(uint32_t aPad, uint32_t aButton);
		static bool					ButtonDown				(uint32_t aPad, uint32_t aButton);
	
		static uint32_t				GetAnyButton			(uint32_t aPad);
		static std::string			GetButtonName			(uint32_t aButton);
		static std::string			GetButtonImage			(uint32_t aButton);

		static void					RumbleOn				(uint32_t aSmall, uint32_t aLarge);
	
	protected:
		static const uint32_t		MAXPADS = 4;
		static const uint32_t		BUTTONS = 24;
		static const uint32_t		AXISCOUNT = 4;

		static void					Assert					(uint32_t aPad, uint32_t aButton, uint32_t aAxis = 0);

		static CellPadInfo2			PadInfo;
		static CellPadData			CurrentState[MAXPADS];

		static uint32_t				HeldState[MAXPADS][BUTTONS];
		static uint32_t				SingleState[MAXPADS][BUTTONS];	

		static uint32_t				Small;
		static uint32_t				Large;
};

