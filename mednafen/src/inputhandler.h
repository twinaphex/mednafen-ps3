#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

class												MednafenInputItem : public ListItem
{
	public:
													MednafenInputItem				(const std::string& aNiceName, const std::string& aRealName);
		virtual										~MednafenInputItem				(){};

		std::string									GetRealName						();

	protected:
		std::string									RealName;
};

class												MednafenInputSelect : public WinterfaceList
{
	public:
													MednafenInputSelect				(const std::vector<std::string>& aInputNames, const std::vector<std::string>& aRealNames);
		virtual										~MednafenInputSelect			(){};
};

class												MednafenSettingButton : public Winterface
{
	public:
													MednafenSettingButton			(const std::string& aInputName, const std::string& aImage);
		virtual										~MednafenSettingButton			(){}
		
		virtual bool								Input							();
		virtual bool								DrawLeft						();
		
		uint32_t									GetButton						();
		
	protected:
		std::string									InputName;
		std::string									Image;
		int32_t										Button;
};

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
			uint32_t								Type;
			uint32_t								Button;
			uint32_t								Index;
			uint32_t								ConfigOrder;
		};

	protected:

		static const InputDeviceInputInfoStruct*	GetGamepad						(const InputInfoStruct* aInfo, const char* aName, uint32_t& aInputCount);
		static void									BuildShifts						(const InputDeviceInputInfoStruct* aInfo, uint32_t aButtonCount, std::vector<InputInfo>& aInputs);
	
		MDFNGI*										GameInfo;

		std::string									PadType;
		uint8_t										ControllerBits[16][256];
		std::vector<InputInfo>						Inputs;
};

#endif

