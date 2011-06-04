#ifndef SDLINPUT_H
#define SDLINPUT_H

enum
{
	ES_BUTTON_UP = 0xFFFFFF00, ES_BUTTON_DOWN, ES_BUTTON_LEFT, ES_BUTTON_RIGHT, ES_BUTTON_ACCEPT, ES_BUTTON_CANCEL,
	ES_BUTTON_SHIFT, ES_BUTTON_TAB, ES_BUTTON_AUXLEFT1, ES_BUTTON_AUXRIGHT1, ES_BUTTON_AUXLEFT2, ES_BUTTON_AUXRIGHT2, 
	ES_BUTTON_AUXLEFT3, ES_BUTTON_AUXRIGHT3, 
};

class				ESInput
{
	friend class							ESVideo;

	public:
		static void							Initialize				();
		static void							Shutdown				();
					
		static uint32_t						PadCount				();
		static void							Reset					();
		static void							Refresh					();
		
		static int32_t						GetAxis					(uint32_t aPad, uint32_t aAxis);

		static bool							ButtonPressed			(uint32_t aPad, uint32_t aButton);
		static bool							ButtonDown				(uint32_t aPad, uint32_t aButton);
		
		static uint32_t						GetAnyButton			(uint32_t aPad);
		static std::string					GetButtonName			(uint32_t aButton);
		static std::string					GetButtonImage			(uint32_t aButton);

		static void							RumbleOn				(uint32_t aBig, uint32_t aSmall) {};
	
	protected:
		static const uint32_t				MAXKEYS = 512;
		static const uint32_t				MAXPADS = 4;
		static const uint32_t				BUTTONS = 128;
		static const uint32_t				AXISCOUNT = 8;
	
		static void							Assert					(uint32_t aPad, uint32_t aButton);
		static bool							IsJoystickButton		(uint32_t aButton);
		
		static std::vector<SDL_Joystick*>	Joysticks;
		static uint32_t						ESInputs[14];

		static uint32_t						KeyState[MAXKEYS];
		static uint32_t						KeySingle[MAXKEYS];

		static uint32_t						HeldState[MAXPADS][BUTTONS];
		static uint32_t						SingleState[MAXPADS][BUTTONS];	
};

#endif

