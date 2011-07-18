#pragma once

#define ES_BUTTON_UP 0xFFFFFF00ul
#define ES_BUTTON_DOWN 0xFFFFFF01ul
#define ES_BUTTON_LEFT 0xFFFFFF02ul
#define ES_BUTTON_RIGHT 0xFFFFFF03ul
#define ES_BUTTON_ACCEPT 0xFFFFFF04ul
#define ES_BUTTON_CANCEL 0xFFFFFF05ul
#define ES_BUTTON_SHIFT 0xFFFFFF06ul
#define ES_BUTTON_TAB 0xFFFFFF07ul
#define ES_BUTTON_AUXLEFT1 0xFFFFFF08ul
#define ES_BUTTON_AUXRIGHT1 0xFFFFFF09ul
#define ES_BUTTON_AUXLEFT2 0xFFFFFF0Aul
#define ES_BUTTON_AUXRIGHT2 0xFFFFFF0Bul
#define ES_BUTTON_AUXLEFT3 0xFFFFFF0Cul
#define ES_BUTTON_AUXRIGHT3 0xFFFFFF0Dul

class				ESInput
{
	public:	//Helpers
		struct								Button
		{
			bool							Pressed;
			bool							Inspected;
			bool							Jammed;
			uint32_t						PressedTime;

			typedef	bool					(*RefreshFunction)		(uint32_t, uint32_t, uint32_t);
			uint32_t						User1, User2, User3;
			
			bool							(*Refresh)				(uint32_t aUser1, uint32_t aUser2, uint32_t aUser3);

											Button					(RefreshFunction aCallback, uint32_t aUser1, uint32_t aUser2, uint32_t aUser3) :
				Pressed(false),	
				Inspected(true),
				Jammed(true),
				PressedTime(0),
				User1(aUser1),
				User2(aUser2),
				User3(aUser3),
				Refresh(aCallback)
			{

			}

			///Set the pressed state of the button.
			///@param aPressed True if the Button is pressed.
			void							SetState				(bool aPressed)
			{
				//Update jammed state
				Jammed = (Jammed && aPressed);

				//Only if the state is changed
				if(Pressed != aPressed)
				{
					Pressed = aPressed;
					Inspected = false;
//					PressedTime = Utility::GetTicks();
				}
			}

			///Force the button to report not being pressed until the next time it is released.
			void							Reset					()
			{
				Jammed = true;
			}

			///Get the pressed state of the button.
			///@return True if the Button is pressed.
			bool							GetState				()
			{
				return Jammed ? false : Pressed;
			}

			///Get the pressed state of the button.
			///@return True if the Button is pressed.
			bool							GetStateInspected		()
			{
				bool result = Inspected ? false : Pressed;
				Inspected = true;
				return Jammed ? false : result;
			}
		};
		typedef std::vector<Button>			InputDevice;
		typedef std::vector<InputDevice>	InputDeviceList;

		///Initialize the libes Input system.
		static void							Initialize				();

		///Shutdown the libes Input system.
		static void							Shutdown				();

		static uint32_t						WaitForESKey			(uint32_t aPad, bool aGuarantee);
					
		static uint32_t						PadCount				() {return Inputs.size() ? Inputs.size() : 1;};
		static void							Reset					();
		static void							Refresh					();
		
		static int32_t						GetAxis					(uint32_t aPad, uint32_t aAxis) {return 0;}

		static inline bool					ButtonPressed			(uint32_t aPad, uint32_t aButton)
		{
			//Translate ES keys
			if(aButton >= ES_BUTTON_UP && aButton <= ES_BUTTON_AUXRIGHT3)
			{
				aButton = ESKeyIndex[aButton & 0xFF];
			}

			//Fetch from main device
			if(aButton < 0x10000 && aPad < Inputs.size())
			{
				return (aButton < Inputs[aPad].size()) ? Inputs[aPad][aButton].GetState() : false;
			}
			//Fetch from sub device
			else if(((aButton - 0x10000) / 0x10000) < SubInputs.size())
			{
				uint32_t device = ((aButton - 0x10000) / 0x10000);
				uint32_t button = aButton & 0xFFFF;
				return (button < SubInputs[device].size()) ? SubInputs[device][button].GetState() : false;
			}
		}

		static inline bool					ButtonDown				(uint32_t aPad, uint32_t aButton)
		{
			//Translate ES keys
			if(aButton >= ES_BUTTON_UP && aButton <= ES_BUTTON_AUXRIGHT3)
			{
				aButton = ESKeyIndex[aButton & 0xFF];
			}

			//Fetch from main device
			if(aButton < 0x10000 && aPad < Inputs.size())
			{
				return (aButton < Inputs[aPad].size()) ? Inputs[aPad][aButton].GetStateInspected() : false;
			}
			//Fetch from sub device
			else if(((aButton - 0x10000) / 0x10000) < SubInputs.size())
			{
				uint32_t device = ((aButton - 0x10000) / 0x10000);
				uint32_t button = aButton & 0xFFFF;
				return (button < SubInputs[device].size()) ? SubInputs[device][button].GetStateInspected() : false;
			}
		}	
	
		static uint32_t						GetAnyButton			(uint32_t aPad);
		static void							RumbleOn				(uint32_t aBig, uint32_t aSmall);

	private:
		static InputDeviceList				Inputs;
		static InputDeviceList				SubInputs;
		static uint32_t						ESKeyIndex[14];
};

