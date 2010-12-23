#ifndef ESINPUT_H
#define ESINPUT_H


enum
{
	ES_BUTTON_UP = SDLK_UP, ES_BUTTON_DOWN = SDLK_DOWN, ES_BUTTON_LEFT = SDLK_LEFT, ES_BUTTON_RIGHT = SDLK_RIGHT, ES_BUTTON_ACCEPT = SDLK_RETURN, ES_BUTTON_CANCEL = SDLK_ESCAPE,
	ES_BUTTON_SHIFT = SDLK_SPACE, ES_BUTTON_TAB = SDLK_TAB, ES_BUTTON_AUXLEFT1 = SDLK_F1, ES_BUTTON_AUXRIGHT1 = SDLK_F2, ES_BUTTON_AUXLEFT2 = SDLK_F3, ES_BUTTON_AUXRIGHT2 = SDLK_F4, 
	ES_BUTTON_AUXLEFT3 = SDLK_F5, ES_BUTTON_AUXRIGHT3 = SDLK_F6, 
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
		static const uint32_t		BUTTONS = 256;
		static const uint32_t		AXISCOUNT = 4;

//HACK:
		static void					Refresh					();
	
	
	protected:
		static void					ProcessInputThread		(uint64_t aBcD);

		static uint32_t				HeldState[MAXPADS][BUTTONS];
		static uint32_t				HeldTime[MAXPADS][BUTTONS];		
		static uint32_t				SingleState[MAXPADS][BUTTONS];	
};

#endif

