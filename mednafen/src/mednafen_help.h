#ifndef MEDNAFEN_HELP_H
#define MEDNAFEN_HELP_H

class	MednafenEmu
{
	public:
		static void						Init				();
		static void						Quit				();
							
		static void						DisplayMessage		(std::string aMessage);
							
		static void						LoadGame			(std::string aFileName, void* aData = 0, int aSize = 0);
		static void						CloseGame			();
		static bool						IsGameLoaded		();
		
		static void						Frame				();
		static void						DummyFrame			();

		static void						GenerateSettings	(std::vector<MDFNSetting>& aSettings);
		
		static void						DoCommand			(std::string aName);


	protected:
		static bool						IsInitialized;
		static bool						IsLoaded;
	
		static Texture*					Buffer;
		static MDFN_Surface*			Surface;
		static InputHandler*			Inputs;		
		static TextViewer*				TextFile;
		static FastCounter				Counter;
	
		static MDFNGI*					GameInfo;

		static std::vector<MDFNSetting>	Settings;
		static std::string				SettingHeader;
	
		static MDFN_Rect				VideoWidths[512];
		static MDFN_Rect				DisplayRect;

		static int16					Samples[2*48000];
		static int16_t					SamplesUp[48000];
};

#endif
