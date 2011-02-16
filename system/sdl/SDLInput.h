#ifndef SDLINPUT_H
#define SDLINPUT_H

enum
{
	ES_BUTTON_UP = 0xFFFFFF00, ES_BUTTON_DOWN, ES_BUTTON_LEFT, ES_BUTTON_RIGHT, ES_BUTTON_ACCEPT, ES_BUTTON_CANCEL,
	ES_BUTTON_SHIFT, ES_BUTTON_TAB, ES_BUTTON_AUXLEFT1, ES_BUTTON_AUXRIGHT1, ES_BUTTON_AUXLEFT2, ES_BUTTON_AUXRIGHT2, 
	ES_BUTTON_AUXLEFT3, ES_BUTTON_AUXRIGHT3, 
};

class				SDLInput : public ESInput
{
	friend class							SDLVideo;

	public:
											SDLInput				();
											~SDLInput				();
					
		uint32_t							PadCount				();
		void								Reset					();
		void								Refresh					();
		
		int32_t								GetAxis					(uint32_t aPad, uint32_t aAxis);

		bool								ButtonPressed			(uint32_t aPad, uint32_t aButton);
		bool								ButtonDown				(uint32_t aPad, uint32_t aButton);
		
		uint32_t							GetAnyButton			(uint32_t aPad);
		std::string							GetButtonName			(uint32_t aButton);
		std::string							GetButtonImage			(uint32_t aButton);
	
	protected:
		static const uint32_t				MAXKEYS = 512;
		static const uint32_t				MAXPADS = 4;
		static const uint32_t				BUTTONS = 128;
		static const uint32_t				AXISCOUNT = 8;
	
		void								Assert					(uint32_t aPad, uint32_t aButton);
		bool								IsJoystickButton		(uint32_t aButton);
		
		std::vector<SDL_Joystick*>			Joysticks;
		uint32_t							ESInputs[14];

		uint32_t							KeyState[MAXKEYS];
		uint32_t							KeySingle[MAXKEYS];

		uint32_t							HeldState[MAXPADS][BUTTONS];
		uint32_t							SingleState[MAXPADS][BUTTONS];	
};

#endif

