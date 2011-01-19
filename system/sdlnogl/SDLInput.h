#ifndef SDLINPUT_H
#define SDLINPUT_H

enum
{
	ES_BUTTON_UP = 0x10000 | SDLK_UP, ES_BUTTON_DOWN = 0x10000 | SDLK_DOWN, ES_BUTTON_LEFT = 0x10000 | SDLK_LEFT, ES_BUTTON_RIGHT = 0x10000 | SDLK_RIGHT, ES_BUTTON_ACCEPT = 0x10000 | SDLK_RETURN, ES_BUTTON_CANCEL = 0x10000 | SDLK_ESCAPE,
	ES_BUTTON_SHIFT = 0x10000 | SDLK_SPACE, ES_BUTTON_TAB = 0x10000 | SDLK_TAB, ES_BUTTON_AUXLEFT1 = 0x10000 | SDLK_F1, ES_BUTTON_AUXRIGHT1 = 0x10000 | SDLK_F2, ES_BUTTON_AUXLEFT2 = 0x10000 | SDLK_F3, ES_BUTTON_AUXRIGHT2 = 0x10000 | SDLK_F4, 
	ES_BUTTON_AUXLEFT3 = 0x10000 | SDLK_F5, ES_BUTTON_AUXRIGHT3 = 0x10000 | SDLK_F6, 
};

class				SDLInput : public ESInput
{
	friend class							SDLVideo;

	public:
											SDLInput				();
											~SDLInput				();
					
		uint32_t							PadCount				();
		void								Reset					();
		
		int32_t								GetAxis					(uint32_t aPad, uint32_t aAxis);

		bool								ButtonPressed			(uint32_t aPad, uint32_t aButton);
		bool								ButtonDown				(uint32_t aPad, uint32_t aButton);
		
		uint32_t							GetAnyButton			(uint32_t aPad);
		std::string							GetButtonName			(uint32_t aButton);
	
	protected:
		static const uint32_t				MAXKEYS = 512;
		static const uint32_t				MAXPADS = 4;
		static const uint32_t				BUTTONS = 128;
		static const uint32_t				AXISCOUNT = 8;
	
		void								Assert					(uint32_t aPad, uint32_t aButton);
		bool								IsJoystickButton		(uint32_t aButton);
		void								Refresh					();
		
		std::vector<SDL_Joystick*>			Joysticks;

		uint32_t							KeyState[MAXKEYS];
		uint32_t							KeySingle[MAXKEYS];

		uint32_t							HeldState[MAXPADS][BUTTONS];
		uint32_t							SingleState[MAXPADS][BUTTONS];	
};

#endif

