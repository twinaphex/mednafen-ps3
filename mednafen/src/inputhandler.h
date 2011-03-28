#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

class												InputHandler
{
	public:
													InputHandler					(MDFNGI* aGameInfo);
		virtual										~InputHandler					();
								
		void										Process							();
		
		void										Configure						();
		void										ReadSettings					();
		static void									GenerateSettings				(std::vector<MDFNSetting>& aSettings);
		
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
		static bool									GetButton						(void* aUserData, Summerface_Ptr aInterface, const std::string& aWindow);	//For SummerfaceStaticInputConduit
	
		MDFNGI*										GameInfo;

		std::string									PadType;
		uint8_t										ControllerBits[16][256];
		std::vector<InputInfo>						Inputs;
		bool										RapidOn;
};

#endif

