#ifndef MEDNAFEN_HELP_H
#define MEDNAFEN_HELP_H

DEFINE_PTR_TYPE(MDFN_Surface);
DEFINE_PTR_TYPE(Texture);
DEFINE_PTR_TYPE(InputHandler);
DEFINE_PTR_TYPE(Filter);

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
		static void						SetPause			(bool aPause);
		static uint32_t					GetPixel			(uint32_t aX, uint32_t aY);

	public:		//Inlines
		static bool						IsGameLoaded		() {return IsInitialized && IsLoaded;}
		static bool						IsEmuInitialized	() {return IsInitialized;}
		static bool						IsGamePaused		() {return IsPaused;}
		static const MDFNGI*			GetGameInfo			() {return GameInfo;}
		static uint32_t					GetFrameCount		() {return FrameCount;}
		static uint8_t					ReadROM				(uint32_t aOffset) {return (ROMData && (ROMSize > aOffset)) ? ROMData[aOffset] : 0;};
		static const InputHandler*		GetInputs			() {return Inputs ? Inputs.get() : 0;}

		static void						DisplayMessage		(std::string aMessage) {Message = aMessage; MessageTime = MDFND_GetTime();}
		
		static bool						DummyFrame			() {if(IsGameLoaded() && !SuspendDraw){Blit(0, 0, 0, 0, true); return true;} return false;}
		static void						ReadSettings		(bool aOnLoad = false);

	protected:	//Internals
		static void						GenerateSettings	(std::vector<MDFNSetting>& aSettings);

	protected:
		static bool						IsInitialized;
		static bool						IsLoaded;
		static bool						IsPaused;
	
		static Texture_Ptr				Buffer;
		static MDFN_Surface_Ptr			Surface;
		static bool						SuspendDraw;

		static MDFNGI*					GameInfo;
		static uint8_t*					ROMData;
		static uint32_t					ROMSize;
		static InputHandler_Ptr			Inputs;		
		static FastCounter				Counter;
		static EmuRealSyncher			Syncher;
#ifndef NO_LUA
		static LuaScripter*				Lua; //TODO: Make shared_ptr
#endif
		
		static std::string				Message;
		static uint32_t					MessageTime;
		static bool						RecordingVideo;
		static bool						RecordingWave;

		static std::vector<MDFNSetting>	Settings;

		static EmulateSpecStruct		EmulatorSpec;
		static MDFN_Rect				VideoWidths[512];
		static int16					Samples[48000];
		static int16_t					SamplesUp[48000];
		static bool						SkipNext;
		static uint32_t					SkipCount;
		static uint32_t					FrameCount;

		//Setting cache
		static bool						RewindSetting;
		static bool						DisplayFPSSetting;
		static int32_t					AspectSetting;
		static int32_t					UnderscanSetting;
		static std::string				ShaderSetting;
		static uint32_t					ShaderPrescaleSetting;
		static Area						UndertuneSetting;
		static bool						VsyncSetting;
		static std::string				BorderSetting;
};

#endif


