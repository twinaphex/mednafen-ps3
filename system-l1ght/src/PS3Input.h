#ifndef PS3INPUT_H
#define PS3INPUT_H


enum
{
	PS3_BUTTON_START, PS3_BUTTON_SELECT, PS3_BUTTON_SQUARE, PS3_BUTTON_CROSS, 
	PS3_BUTTON_TRIANGLE, PS3_BUTTON_CIRCLE, PS3_BUTTON_UP, PS3_BUTTON_DOWN, 
	PS3_BUTTON_LEFT, PS3_BUTTON_RIGHT, PS3_BUTTON_L1, PS3_BUTTON_L2, 
	PS3_BUTTON_L3, PS3_BUTTON_R1, PS3_BUTTON_R2, PS3_BUTTON_R3, PS3_BUTTON_COUNT
};

extern std::string ButtonNames[16];

enum
{
	PS3_AXIS_LEFT_Y, PS3_AXIS_LEFT_X, PS3_AXIS_RIGHT_Y, PS3_AXIS_RIGHT_X, PS3_AXIS_COUNT
};

class				PS3Input
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

