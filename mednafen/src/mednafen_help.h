#ifndef MEDNAFEN_HELP_H
#define MEDNAFEN_HELP_H

class	MednafenEmu
{
	public:
		static void						Init				();
		static void						Quit				();
							
		static void						LoadGame			(std::string aFileName, void* aData = 0, int aSize = 0);
		static void						CloseGame			();
		
		static void						Frame				();
		static void						Blit				();
		
		static void						DoCommand			(const char* aName);

	public:		//Inlines
		static bool						IsGameLoaded		()
		{
			return IsLoaded;
		}
		
		static bool						IsEmuInitialized	()
		{
			return IsInitialized;
		}

		static void						DisplayMessage		(std::string aMessage)
		{
			if(IsInitialized && IsLoaded)
			{
				Message = aMessage;
				MessageTime = MDFND_GetTime();
			}
		}
		
		static void						DummyFrame			()
		{
			if(IsInitialized && IsLoaded)
			{
				Blit();
			}
			
			es_video->Flip();					
		}

	protected:	//Internals
		static void						GenerateSettings	(std::vector<MDFNSetting>& aSettings);

	protected:
		static bool						IsInitialized;
		static bool						IsLoaded;
	
		static Texture*					Buffer;
		static MDFN_Surface*			Surface;

		static MDFNGI*					GameInfo;
		static InputHandler*			Inputs;		
		static TextViewer*				TextFile;
		static Filter*					Scaler;
		static FastCounter				Counter;
		static EmuRealSyncher			Syncher;
		
		static std::string				Message;
		static uint32_t					MessageTime;
		static bool						PCESkipHack;
		static bool						RewindEnabled;

		static std::vector<MDFNSetting>	Settings;

		static EmulateSpecStruct		EmulatorSpec;
		static MDFN_Rect				VideoWidths[512];
		static int16					Samples[48000];
		static int16_t					SamplesUp[48000];
};

#endif


