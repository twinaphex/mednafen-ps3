#pragma once

class												InputEnumerator
{
	public:
		virtual void								System							(const MDFNGI* aDescription) = 0;
		virtual bool								Port							(const InputPortInfoStruct* aDescription) = 0;
		virtual bool								Device							(const InputDeviceInfoStruct* aDescription) = 0;
		virtual void								Button							(const InputDeviceInputInfoStruct* aDescription) = 0;
		virtual void								Finish							() {};
};

class												InputHandler
{
	typedef SummerfaceItemUser<std::string>			InputItem;
	typedef AnchoredListView<InputItem>				InputList;

	public:
													InputHandler					(MDFNGI* aGameInfo);
		virtual										~InputHandler					() {};
								
		static void									EnumerateInputs					(const MDFNGI* aSystem, InputEnumerator* aEnumerator);

		void										Process							();
		
		void										Configure						();
		void										ReadSettings					();

		static void									GenerateSettings				(std::vector<MDFNSetting>& aSettings);

		//Doc Note: delete[] the result, not free!
		static MDFNSetting_EnumList*				BuildPortEnum					(const InputPortInfoStruct& aPort);
		static int									GetButton						(void* aUserData, Summerface* aInterface, const std::string& aWindow, uint32_t aButton);	//For SummerfaceStaticInputConduit

		uint8_t*									GetRawBits						(uint32_t aPort) {assert(aPort < 16); return ControllerBits[aPort];}

	public:
		struct										InputInfo
		{
			uint32_t								BitOffset;
			uint32_t								Button;
			uint32_t								RapidButton;
			uint32_t								Index;
			const InputDeviceInputInfoStruct*		Data;
		};

	private:
		MDFNGI*										GameInfo;
		uint32_t									RapidCount;

		std::vector<InputInfo>						Inputs;
		uint8_t										ControllerBits[16][256];
};


