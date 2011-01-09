#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

class												MednafenInputItem : public ListItem
{
	public:
													MednafenInputItem				(const std::string& aNiceName, const std::string& aRealName);

		std::string									GetRealName						();

	protected:
		std::string									RealName;
};

class												MednafenInputSelect : public WinterfaceList
{
	public:
													MednafenInputSelect				(const std::vector<std::string>& aInputNames, const std::vector<std::string>& aRealNames);
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
													~InputHandler					();
								
		void										Process							();
		
		void										Configure						();
		void										ReadSettings					();
		static void									GenerateSettings				(std::vector<MDFNSetting>& aSettings);
		
	protected:
		static const InputDeviceInputInfoStruct*	GetGamepad						(const InputInfoStruct* aInfo, const char* aName, uint32_t& aInputCount);
		static void									BuildShifts						(const InputDeviceInputInfoStruct* aInfo, uint32_t aButtonCount, uint32_t aOrder[32][2]);
		static bool									IsInputSupported				(const InputDeviceInputInfoStruct* aInfo, uint32_t aButtonCount);
	
		MDFNGI*										GameInfo;

		std::string									PadType;
		uint32_t									ButtonCount;
		uint32_t									ControllerBits[16];
		uint32_t									Button[32][2];
};

#endif

