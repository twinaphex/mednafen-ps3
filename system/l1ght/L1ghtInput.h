#ifndef L1GHTINPUT_H
#define L1GHTINPUT_H


enum
{
	ES_BUTTON_UP = 6, ES_BUTTON_DOWN = 7, ES_BUTTON_LEFT = 8, ES_BUTTON_RIGHT = 9, ES_BUTTON_ACCEPT = 3, ES_BUTTON_CANCEL = 5,
	ES_BUTTON_SHIFT = 2, ES_BUTTON_TAB = 4, ES_BUTTON_AUXLEFT1 = 10, ES_BUTTON_AUXRIGHT1 = 13, ES_BUTTON_AUXLEFT2 = 11, ES_BUTTON_AUXRIGHT2 = 14, 
	ES_BUTTON_AUXLEFT3 = 12, ES_BUTTON_AUXRIGHT3 = 15 
};

class				L1ghtInput : public ESInput
{
	public:
									L1ghtInput				();
									~L1ghtInput				();
					
		uint32_t					PadCount				();
		void						Reset					();
		
		int32_t						GetAxis					(uint32_t aPad, uint32_t aAxis);

		bool						ButtonPressed			(uint32_t aPad, uint32_t aButton);
		bool						ButtonDown				(uint32_t aPad, uint32_t aButton);
	
		uint32_t					GetAnyButton			(uint32_t aPad);
		std::string					GetButtonName			(uint32_t aButton);
		std::string					GetButtonImage			(uint32_t aButton);

		void						RumbleOn				(bool aRumble);
	
	protected:
		static const uint32_t		MAXPADS = 4;
		static const uint32_t		BUTTONS = 16;
		static const uint32_t		AXISCOUNT = 4;

		void						Assert					(uint32_t aPad, uint32_t aButton, uint32_t aAxis = 0);

		static void					ProcessInputThread		(uint64_t aBcD);
		void						Refresh					();
		
		sys_ppu_thread_t			ThreadID;
		volatile bool				ThreadDie;
		
		PadInfo						Info;
		PadData						CurrentState[MAXPADS];

		uint32_t					HeldState[MAXPADS][BUTTONS];
		uint32_t					SingleState[MAXPADS][BUTTONS];	

		bool						Rumble;
};

#endif

