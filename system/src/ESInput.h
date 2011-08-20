#pragma once

#define ES_BUTTON_UP		0xFFFFFF00ul
#define ES_BUTTON_DOWN		0xFFFFFF01ul
#define ES_BUTTON_LEFT		0xFFFFFF02ul
#define ES_BUTTON_RIGHT		0xFFFFFF03ul
#define ES_BUTTON_ACCEPT	0xFFFFFF04ul
#define ES_BUTTON_CANCEL	0xFFFFFF05ul
#define ES_BUTTON_SHIFT		0xFFFFFF06ul
#define ES_BUTTON_TAB		0xFFFFFF07ul
#define ES_BUTTON_AUXLEFT1	0xFFFFFF08ul
#define ES_BUTTON_AUXRIGHT1	0xFFFFFF09ul
#define ES_BUTTON_AUXLEFT2	0xFFFFFF0Aul
#define ES_BUTTON_AUXRIGHT2	0xFFFFFF0Bul
#define ES_BUTTON_AUXLEFT3	0xFFFFFF0Cul
#define ES_BUTTON_AUXRIGHT3	0xFFFFFF0Dul

class				ESInput
{
	public:	//Helpers
		///Class holding state and methods for a single input button.
		class								Button
		{
			friend class					ESInput;

			public:
				///Callback type for refreshing the Button's pressed state.
				typedef	bool				(*RefreshFunction)		(uint32_t, uint32_t, uint32_t);
			
				///Create a new Button object.
				///@param aCallback Function that will be called to determine the Button's state.
				///@param aUser1 User data that will be passed to the callback.
				///@param aUser2 User data that will be passed to the callback.
				///@param aUser3 User data that will be passed to the callback.
											Button					(RefreshFunction aCallback, uint32_t aUser1, uint32_t aUser2, uint32_t aUser3, const std::string& aName) :
					Name(aName),
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
				void						SetState				(bool aPressed); //External

				///Get the name of the button.
				///@return The name of the button.
				const std::string&			GetName					()
				{
					return Name;
				}

				///Force the button to report not being pressed until the next time it is released.
				void						Reset					()
				{
					Jammed = true;
				}

				///Get the pressed state of the button.
				///@return True if the Button is pressed.
				bool						GetState				()
				{
					return Jammed ? false : Pressed;
				}

				///Get the pressed state of the button.
				///@return True if the Button is pressed.
				bool						GetStateInspected		()
				{
					bool result = Inspected ? false : Pressed;
					Inspected = true;
					return Jammed ? false : result;
				}

				///Get the state of the button with a repeating pulse.
				///@return True if the Button is pressed.
				bool						GetStateRepeat			();

			private:
				std::string					Name;					///<Friendly name of the Button.

				bool						Pressed;				///<Set to true when the Button is pressed, false when the button is released.
				bool						Inspected;				///<Set to true after a call to GetStateInspected, false when the button is released.
				bool						Jammed;					///<Set to true after a call to Reset, false when the button is released.
				uint32_t					PressedTime;			///<Internal timer used for GetStateRepeat.
				uint32_t					User1;					///<User data for the callback.
				uint32_t					User2;					///<User data for the callback.
				uint32_t					User3;					///<User data for the callback.

				///Callback function to refresh the Button's state.
				///@param aUser1 A 32-bit user data.
				///@param aUser2 A 32-bit user data.
				///@param aUser3 A 32-bit user data.
				///@return True if the Button is pressed, false if it is released.
				bool						(*Refresh)				(uint32_t aUser1, uint32_t aUser2, uint32_t aUser3);
		};
		typedef std::vector<Button>			InputDevice;
		typedef std::vector<InputDevice>	InputDeviceList;

		///Initialize the libes Input system.
		static void							Initialize				();

		///Shutdown the libes Input system.
		static void							Shutdown				();

		///Wait for a system key to be pressed.
		///@param aPad Input port to wait for.
		///@param aGuarantee If true the routine will loop until a key is pressed.
		///@return An ES_BUTTON constant if a key was found, 0xFFFFFFFF otherwise.
		static uint32_t						WaitForESKey			(uint32_t aPad, bool aGuarantee);
					
		///Get the number of available input devices.
		///@return The number of available input devices.
		static uint32_t						PadCount				() {return Inputs.size() ? Inputs.size() : 1;};

		///Reset the state of all buttons. All buttons will report being as being not active until they are released
		///and pressed again.
		static void							Reset					();

		///Poll the input system to update button status.
		static void							Refresh					();

		///Determine if a button is being held down.
		///@param aPad Input device to check.
		///@param aButton Button to check on device.
		///@return True if the button is held down.
		static inline bool					ButtonPressed			(uint32_t aPad, uint32_t aButton)
		{
			Button* button = GetButton(aPad, aButton);
			return button && button->GetState();
		}

		///Determine if a button was pressed. This function returns true only once for every time the button is pressed.
		///@param aPad Input device to check.
		///@param aButton Button to check on device.
		///@return True if the button was pressed.
		static inline bool					ButtonDown				(uint32_t aPad, uint32_t aButton)
		{
			Button* button = GetButton(aPad, aButton);
			return button && button->GetStateInspected();
		}

		static std::string					ButtonName				(uint32_t aPad, uint32_t aButton)
		{
			const char* const esNames[] = {"UP", "DOWN", "LEFT", "RIGHT", "ACCEPT", "CANCEL", "SHIFT", "TAB", "L1", "R1", "L2", "R2", "L3", "R3"};

			if(aButton >= ES_BUTTON_UP && aButton <= ES_BUTTON_AUXRIGHT3)
			{
				return esNames[aButton - ES_BUTTON_UP];
			}
			else
			{
				Button* button = GetButton(aPad, aButton);
				return button ? button->GetName() : "INVALID BUTTON";
			}
		}

		///Scan a device and determine if any button, system or otherwise, is pressed.
		///@param aPad Input device to scan.
		///@return A button ID or 0xFFFFFFFF if no buttons were pressed.
		static uint32_t						GetAnyButton			(uint32_t aPad);

		///Unused
		static void							RumbleOn				(uint32_t aBig, uint32_t aSmall) {};

	private:
		///Get a pointer to a Button on an input device.
		///@param aPad Input device to scan.
		///@param aButton Button index to retrieve.
		///@return A pointer to a Button structure, or NULL if parameters are invalid.
		static Button*						GetButton				(uint32_t aPad, uint32_t aButton)
		{
			//Translate ES keys
			if(aButton >= ES_BUTTON_UP && aButton <= ES_BUTTON_AUXRIGHT3)
			{
				aButton = ESKeyIndex[aButton & 0xFF];
			}

			//Fetch from main device
			if(aButton < 0x10000 && aPad < Inputs.size() && aButton < Inputs[aPad].size())
			{
				return &Inputs[aPad][aButton];
			}
			//Fetch from sub device
			else if(aButton >= 0x10000)
			{
				uint32_t device = ((aButton - 0x10000) / 0x10000);
				uint32_t button = aButton & 0xFFFF;
				return ((device < SubInputs.size()) && (button < SubInputs[device].size())) ? &SubInputs[device][button] : 0;
			}

			return 0;
		}

		static InputDeviceList				Inputs;					///<List of input devices.
		static InputDeviceList				SubInputs;				///<List of sub input devices.
		static uint32_t						ESKeyIndex[14];			///<Mapping of System keys to platform specific keys.
};

///Static class to be implemented by a port.
class										ESInputPlatform
{
	public:
		static void							Initialize				(ESInput::InputDeviceList& aDevices, ESInput::InputDeviceList& aSubDevices, uint32_t aESKeyIndex[14]);
		static void							Shutdown				();
		static void							Refresh					();
};

