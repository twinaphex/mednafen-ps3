#ifndef WiiInput_H
#define WiiInput_H

enum
{
	ES_BUTTON_UP = 11, ES_BUTTON_DOWN = 10, ES_BUTTON_LEFT = 8, ES_BUTTON_RIGHT = 9, ES_BUTTON_ACCEPT = 3, ES_BUTTON_CANCEL = 2,
	ES_BUTTON_SHIFT = 64, ES_BUTTON_TAB, ES_BUTTON_AUXLEFT1, ES_BUTTON_AUXRIGHT1, ES_BUTTON_AUXLEFT2, ES_BUTTON_AUXRIGHT2, 
	ES_BUTTON_AUXLEFT3, ES_BUTTON_AUXRIGHT3, 
};

class				WiiInput : public ESInput
{
	friend class							SDLVideo;

	public:
											WiiInput				();
											~WiiInput				();
					
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
};

#endif

