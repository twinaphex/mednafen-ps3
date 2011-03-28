#ifndef MEDNAFEN_HELP_H
#define MEDNAFEN_HELP_H

class	MednafenEmu
{
	public:
		static void						Init				();
		static void						Quit				();
							
		static void						LoadGame			(std::string aFileName, void* aData = 0, int aSize = 0);
		static void						CloseGame			();
		
		static bool						Frame				();
		static void						Blit				(uint32_t* aPixels = 0, uint32_t aWidth = 0, uint32_t aHeight = 0, uint32_t aPitch = 0, bool aDummy = false);

		static void						DoCommands			();
		static bool						DoCommand			(void* aUserData, Summerface_Ptr aInterface, const std::string& aWindow);

	public:		//Inlines
		static bool						IsGameLoaded		() {return IsInitialized && IsLoaded;}
		static bool						IsEmuInitialized	() {return IsInitialized;}

		static void						DisplayMessage		(std::string aMessage) {Message = aMessage; MessageTime = MDFND_GetTime();}
		
		static bool						DummyFrame			() {if(IsGameLoaded() && !SuspendDraw){Blit(0, 0, 0, 0, true); return true;} return false;}

	protected:	//Internals
		static void						GenerateSettings	(std::vector<MDFNSetting>& aSettings);
		static void						ReadSettings		();

	protected:
		static bool						IsInitialized;
		static bool						IsLoaded;
	
		static Texture*					Buffer;
		static MDFN_Surface*			Surface;
		static bool						SuspendDraw;

		static MDFNGI*					GameInfo;
		static InputHandler*			Inputs;		
		static Filter*					Scaler;
		static FastCounter				Counter;
		static EmuRealSyncher			Syncher;
		
		static std::string				Message;
		static uint32_t					MessageTime;
		static bool						RewindEnabled;
		static bool						RecordingVideo;
		static bool						RecordingWave;

		static std::vector<MDFNSetting>	Settings;

		static EmulateSpecStruct		EmulatorSpec;
		static MDFN_Rect				VideoWidths[512];
		static int16					Samples[48000];
		static int16_t					SamplesUp[48000];
		static bool						SkipNext;
		static uint32_t					SkipCount;

		static bool						RewindSetting;
		static bool						DisplayFPSSetting;
		static bool						FullFrameSetting;
		static int32_t					UnderscanSetting;
		static bool						FilterSetting;
		static int32_t					ScalerSetting;
		static Area						UndertuneSetting;
};

#endif


