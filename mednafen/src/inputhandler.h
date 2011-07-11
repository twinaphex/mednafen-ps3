#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

class												InputHandler
{
	public:
													InputHandler					(MDFNGI* aGameInfo);
		virtual										~InputHandler					() {};
								
		void										Process							();
		
		void										Configure						();
		void										ReadSettings					();
		static void									GenerateSettings				(std::vector<MDFNSetting>& aSettings);

	public: //Inlines
		//TODO: FIXME: These will crash if given bad values
		uint32_t									GetPadCount						() const {return GameInfo->InputInfo->InputPorts;}
		uint32_t									GetButtonCount					(uint32_t aPad) const {return Inputs.size();}
		bool										GetButtonState					(uint32_t aPad, uint32_t aButton) const {return ControllerBits[aPad][Inputs[aButton].BitOffset / 8] & (1 << (Inputs[aButton].BitOffset % 8));}
		const char*									GetButtonName					(uint32_t aPad, uint32_t aButton) const {return Inputs[aButton].Data->SettingName;}

		struct										InputInfo
		{
			uint32_t								BitOffset;
			uint32_t								Button;
			uint32_t								RapidButton;
			uint32_t								Index;
			const InputDeviceInputInfoStruct*		Data;
		};

	protected:
		static void									GetGamepad						(const InputInfoStruct* aInfo, const char* aName, std::vector<InputInfo>& aInputs);
		static int									GetButton						(void* aUserData, Summerface_Ptr aInterface, const std::string& aWindow);	//For SummerfaceStaticInputConduit
	
		MDFNGI*										GameInfo;

		std::string									PadType;
		uint8_t										ControllerBits[16][256];
		std::vector<InputInfo>						Inputs;
		uint32_t									RapidCount;
};

#endif

