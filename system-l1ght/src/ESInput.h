#ifndef ESINPUT_H
#define ESINPUT_H


enum
{
	ES_BUTTON_UP = 6, ES_BUTTON_DOWN = 7, ES_BUTTON_LEFT = 8, ES_BUTTON_RIGHT = 9, ES_BUTTON_ACCEPT = 3, ES_BUTTON_CANCEL = 5,
	ES_BUTTON_SHIFT = 2, ES_BUTTON_TAB = 4, ES_BUTTON_AUXLEFT1 = 10, ES_BUTTON_AUXRIGHT1 = 13, ES_BUTTON_AUXLEFT2 = 11, ES_BUTTON_AUXRIGHT2 = 14, 
	ES_BUTTON_AUXLEFT3 = 12, ES_BUTTON_AUXRIGHT3 = 15 
};

class				ESInput
{
	public:
		static void					Init					();
		static void					Quit					();
					
		static uint32_t				PadCount				();
		static void					Reset					();
		
		static int32_t				GetAxis					(uint32_t aPad, uint32_t aAxis);

		static bool					ButtonPressed			(uint32_t aPad, uint32_t aButton);
		static uint32_t				ButtonTime				(uint32_t aPad, uint32_t aButton);	
		static bool					ButtonDown				(uint32_t aPad, uint32_t aButton);
	
		static uint32_t				GetAnyButton			(uint32_t aPad);
		static std::string			GetButtonName			(uint32_t aButton);
	
	public:
		static const uint32_t		MAXPADS = 4;
		static const uint32_t		BUTTONS = 16;
		static const uint32_t		AXISCOUNT = 4;
	
	
	protected:
		static void					ProcessInputThread		(uint64_t aBcD);
		static void					Refresh					();
		
		static sys_ppu_thread_t		ThreadID;
		static volatile bool		ThreadDie;
		
		static PadInfo				Info;
		static PadData				CurrentState[MAXPADS];

		static uint32_t				HeldState[MAXPADS][BUTTONS];
		static uint32_t				HeldTime[MAXPADS][BUTTONS];		
		static uint32_t				SingleState[MAXPADS][BUTTONS];	
};

#endif

