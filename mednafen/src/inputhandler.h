#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

class												MednafenSettingButton : public Winterface
{
	public:
													MednafenSettingButton			(std::string aAppendToHeader);
		virtual										~MednafenSettingButton			(){}
		
		virtual bool								Input							();
		virtual bool								DrawLeft						();
		
		std::string									GetButton						();
		
	protected:
		std::string									InputName;
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
		static const InputDeviceInputInfoStruct*	GetGamepad						(const InputInfoStruct* aInfo, uint32_t& aInputCount);
		static void									BuildShifts						(const InputDeviceInputInfoStruct* aInfo, uint32_t aButtonCount, uint32_t aOrder[32][2]);
	
		MDFNGI*										GameInfo;
			
		uint32_t									ButtonCount;
		uint32_t									ControllerBits[16];
		uint32_t									PS3Button[32][2];
		
	public:
		static MDFNSetting_EnumList					Buttons[];
};

#endif

