#include <mednafen_includes.h>

namespace
{
	Filter*			BuildFilter			(uint32_t aIndex)
	{
		if(aIndex == 0)	return new Identity();
		if(aIndex == 1)	return new Identity2x();
		if(aIndex == 2) return new Kreed_2xSaI();
		if(aIndex == 3)	return new MaxSt_Hq2x();
		if(aIndex == 4)	return new MaxSt_Hq3x();	
		if(aIndex == 5)	return new Catrom2x();	
		if(aIndex == 6)	return new Catrom3x();		

		return new Identity();
	};

	void			MakeDirectories		()
	{
#ifndef L1GHT
//TODO: Support psl1ght, throw on error
		const char *subs[] = {"mcs", "mcm", "snaps", "palettes", "sav", "cheats", "firmware", "video", "wave"};

		if(mkdir(es_paths->Build("mednafen").c_str(), S_IRWXU) == -1 && errno != EEXIST)
		{
			return;
		}

		char buffer[1024];

		for(int i = 0; i != 9; i ++)
		{
			sprintf(buffer, "mednafen"PSS"%s", subs[i]);
			if(mkdir(es_paths->Build(buffer).c_str(), S_IRWXU) == -1 && errno != EEXIST)
			{
				return;
			}
		}
#endif
	}

	const MDFNSetting_EnumList	FilterEnumList[] =
	{
		{"none", 0, "none", ""},
		{"int2x", 1, "int2x", ""},
		{"k2xSaI", 2, "k2xSaI", ""},
		{"hq2x", 3, "Hq2x", ""},
		{"hq3x", 4, "Hq3x", ""},
		{"catrom2x", 5, "Catrom2x", ""},
		{"catrom3x", 6, "Catrom3x", ""},
		{0, 0, 0, 0}
	};

	#define SETTINGNAME(b) ((std::string(GameInfo->shortname) + ".es." + b).c_str())

	MDFNSetting SystemSettings[] = 
	{
		{"scaler", MDFNSF_NOFLAGS, "Special filter for screen scaling.", NULL, MDFNST_ENUM, "none", NULL, NULL, NULL, NULL, FilterEnumList },	
		{"underscan", MDFNSF_NOFLAGS, "Reduce size of screen to compensate for display overscan.", NULL, MDFNST_INT, "5", "0", "50" },
		{"displayfps", MDFNSF_NOFLAGS, "Display frames per second in corner of screen", NULL, MDFNST_BOOL, "0" },
		{"filter", MDFNSF_NOFLAGS, "Use bilinear filter for display", NULL, MDFNST_BOOL, "0"},
		{"fullframe", MDFNSF_NOFLAGS, "Ignore screen aspect ratio", NULL, MDFNST_BOOL, "0"},
		{"autosave", MDFNSF_NOFLAGS, "Save state at exit", NULL, MDFNST_BOOL, "0"},
		{"rewind", MDFNSF_NOFLAGS, "Enable Rewind Support", NULL, MDFNST_BOOL, "0"}
	};

	MDFNSetting ESSettings[] =
	{
		{"es.bookmarks", MDFNSF_NOFLAGS, "Bookmarks for the file browser.", NULL, MDFNST_STRING, "" },
		{"ftp.es.enable", MDFNSF_NOFLAGS, "Enable Loading from FTP Server.", NULL, MDFNST_BOOL, "0" },
		{"ftp.es.host", MDFNSF_NOFLAGS, "Hostname for FTP Server.", NULL, MDFNST_STRING, "192.168.0.250" },
		{"ftp.es.port", MDFNSF_NOFLAGS, "Port for FTP Server.", NULL, MDFNST_STRING, "21" },
		{"ftp.es.username", MDFNSF_NOFLAGS, "User name for FTP Server.", NULL, MDFNST_STRING, "anonymous" },
		{"ftp.es.password", MDFNSF_NOFLAGS, "Password for FTP Server.", NULL, MDFNST_STRING, "" }
	};
}

void						MednafenEmu::Init				()
{
	if(!IsInitialized)
	{
		//Put settings for the user interface
		for(int i = 0; i != sizeof(ESSettings) / sizeof(MDFNSetting); i++)
		{
			Settings.push_back(ESSettings[i]);
		}

		//Get the external emulators, nestopia and gambatte, vbam for PS3 only
		std::vector<MDFNGI*> externalSystems;
		externalSystems.push_back(nestGetEmulator());
		externalSystems.push_back(gmbtGetEmulator());
		externalSystems.push_back(vbamGetEmulator());
		externalSystems.push_back(pcsxGetEmulator());
		MDFNI_InitializeModules(externalSystems);

		//Make settings for each system
		GenerateSettings(Settings);
		InputHandler::GenerateSettings(Settings);

		//Build directory trees, if these don't exist we can't save games
		MakeDirectories();

		//Initialize mednafen and go
		MDFNI_Initialize(es_paths->Build("mednafen").c_str(), Settings);

		//Create video buffer and surface. 1080p max, we should never come even close to this
		Buffer = es_video->CreateTexture(1920, 1080);
		Surface = new MDFN_Surface(0, 1920, 1080, 1920, MDFN_PixelFormat(MDFN_COLORSPACE_RGB, 16, 8, 0, 24));	
	}

	IsInitialized = true;
}

void						MednafenEmu::Quit				()
{
	if(IsInitialized)
	{
		delete Buffer;
		delete Surface;

		CloseGame();
		MDFNI_Kill();
	}

	IsInitialized = false;
}

void						MednafenEmu::LoadGame			(std::string aFileName, void* aData, int aSize)
{
	if(!IsLoaded && IsInitialized)
	{
		MDFNDES_BlockExit(false);

		if(strstr(aFileName.c_str(), ".cue"))
		{
			GameInfo = MDFNI_LoadCD(0, aFileName.c_str());
		}
		else
		{
			GameInfo = MDFNI_LoadGame(0, aFileName.c_str(), aData, aSize);
		}

		if(GameInfo == 0)
		{
			es_log->Do();
			Exit();
		}

		Buffer->Clear(0);

		Inputs = new InputHandler(GameInfo);
		TextFile = new TextViewer(aFileName + ".txt");

		if(MDFN_GetSettingB(SETTINGNAME("autosave")))
		{
			MDFNI_LoadState(0, "mcq");
		}
	
		IsLoaded = true;
		SkipCount = 0;
		SkipNext = false;

		MDFND_DispMessage((UTF8*)GameInfo->fullname);

		Syncher.SetEmuClock(GameInfo->MasterClock >> 32);
	}
}

void						MednafenEmu::CloseGame			()
{
	if(IsLoaded && IsInitialized)
	{
		if(RecordingVideo)
		{
			MDFNI_StopAVRecord();
			RecordingVideo = false;
		}

		if(RecordingWave)
		{
			MDFNI_StopWAVRecord();
			RecordingWave = false;
		}

		MDFNDES_BlockExit(true);

		if(MDFN_GetSettingB(SETTINGNAME("autosave")))
		{
			MDFNI_SaveState(0, "mcq", 0, 0, 0);
		}
	
		MDFNI_CloseGame();
		
		delete Inputs;
		delete TextFile;
		delete Scaler;
		
		GameInfo = 0;
		Inputs = 0;
		TextFile = 0;
		Scaler = 0;
		
		IsLoaded = false;
		RewindEnabled = false;
	}
}

void						MednafenEmu::Frame				()
{
	if(IsInitialized && IsLoaded)
	{
		bool rewindnow = MDFN_GetSettingB(SETTINGNAME("rewind"));
		if(rewindnow != RewindEnabled)
		{
			MDFNI_EnableStateRewind(rewindnow);
			RewindEnabled = rewindnow;
		}

		Counter.Tick();
	
		Inputs->Process();
	
		memset(VideoWidths, 0xFF, sizeof(MDFN_Rect) * 512);
		memset(&EmulatorSpec, 0, sizeof(EmulateSpecStruct));
		EmulatorSpec.surface = Surface;
		EmulatorSpec.LineWidths = VideoWidths;
		EmulatorSpec.soundmultiplier = Counter.GetSpeed();
		EmulatorSpec.SoundRate = 48000;
		EmulatorSpec.SoundBuf = Samples;
		EmulatorSpec.SoundBufMaxSize = 24000;
		EmulatorSpec.SoundVolume = 1;
		EmulatorSpec.NeedRewind = es_input->ButtonPressed(0, ES_BUTTON_AUXLEFT2);
		EmulatorSpec.skip = SkipNext && ((SkipCount ++) < 4);
		MDFNI_Emulate(&EmulatorSpec);

		if(!EmulatorSpec.skip)
		{
			SkipCount = 0;

			//VIDEO
			Buffer->SetFilter(MDFN_GetSettingB(SETTINGNAME("filter")));

			Blit();			
	
			if(MDFN_GetSettingB(SETTINGNAME("displayfps")))
			{
				char buffer[128];
				snprintf(buffer, 128, "%d", Counter.GetFPS());
				FontManager::GetBigFont()->PutString(buffer, 10, 10, 0xFFFFFFFF);
			}
				
			if(MDFND_GetTime() - MessageTime < 5000)
			{
				FontManager::GetBigFont()->PutString(Message.c_str(), 10, 10 + FontManager::GetBigFont()->GetHeight(), 0xFFFFFFFF);
			}
	
			es_video->Flip();
		}

		//AUDIO
		uint32_t* realsamps = (uint32_t*)Samples;

		if(GameInfo->soundchan == 1)
		{
			for(int i = 0; i != EmulatorSpec.SoundBufSize; i ++)
			{
				SamplesUp[i * 2] = Samples[i];
				SamplesUp[i * 2 + 1] = Samples[i];
			}

			realsamps = (uint32_t*)SamplesUp;
		}

		SkipNext = es_audio->GetBufferAmount() < EmulatorSpec.SoundBufSize * (2 * Counter.GetSpeed());
		es_audio->AddSamples(realsamps, EmulatorSpec.SoundBufSize);
		
		if(es_input->ButtonDown(0, ES_BUTTON_AUXRIGHT3))
		{
			MednafenCommands().Do();
		}
	}
}

void						MednafenEmu::Blit				()
{
	uint32_t aX = VideoWidths[0].w != ~0 ? VideoWidths[0].x : EmulatorSpec.DisplayRect.x;
	uint32_t aY = EmulatorSpec.DisplayRect.y;
	uint32_t aWidth = VideoWidths[0].w != ~0 ? VideoWidths[0].w : EmulatorSpec.DisplayRect.w;
	uint32_t aHeight = EmulatorSpec.DisplayRect.h;

	if(!Scaler)
	{
		Scaler = BuildFilter(MDFN_GetSettingUI(SETTINGNAME("scaler")));
		Scaler->init(aWidth, aHeight);
	}
	
	if(aWidth != Scaler->getWidth() || aHeight != Scaler->getHeight())
	{
		Scaler->init(aWidth, aHeight);
	}

	uint32_t* scaleIn = Scaler->inBuffer();
	uint32_t scaleInP = Scaler->inPitch();
	
	uint32_t* scaleOut = Scaler->outBuffer();
	uint32_t scaleOutP = Scaler->outPitch();
	
	uint32_t* bufferPix = Buffer->GetPixels();
	uint32_t bufferP = Buffer->GetWidth();

	uint32_t finalWidth = aWidth * Scaler->info().outWidth;
	uint32_t finalHeight = aHeight * Scaler->info().outHeight;

	for(int i = 0; i != aHeight; i ++)
	{
		memcpy(&scaleIn[i * scaleInP], &Surface->pixels[(aY + i) * Surface->pitchinpix + aX], aWidth * 4);
	}

	Scaler->filter();
		
	for(int i = 0; i != finalHeight; i ++)
	{
		memcpy(&bufferPix[i * bufferP], &scaleOut[i * scaleOutP], finalWidth * 4);	
	}

	es_video->PresentFrame(Buffer, Area(0, 0, finalWidth, finalHeight), MDFN_GetSettingB(SETTINGNAME("fullframe")), MDFN_GetSettingUI(SETTINGNAME("underscan")));
}

void						MednafenEmu::DoCommand			(const char* aName)
{
	if(IsLoaded && aName)
	{
		if(0 == strcmp(aName, "DoReload"))				ReloadEmulator();
		if(0 == strcmp(aName, "DoSettings"))			MednafenSettings(GameInfo->shortname).Do();
		if(0 == strcmp(aName, "DoReset"))				MDFNI_Reset();
		if(0 == strcmp(aName, "DoScreenShot"))			MDFNI_SaveSnapshot(Surface, &EmulatorSpec.DisplayRect, VideoWidths);
		if(0 == strcmp(aName, "DoSaveState"))			MDFNI_SaveState(0, 0, Surface, &EmulatorSpec.DisplayRect, VideoWidths);
		if(0 == strcmp(aName, "DoLoadState"))			MDFNI_LoadState(0, 0);
		if(0 == strcmp(aName, "DoSaveStateMenu"))		MednafenStateMenu(false).Do();
		if(0 == strcmp(aName, "DoLoadStateMenu"))		MednafenStateMenu(true).Do();
		if(0 == strcmp(aName, "DoInputConfig"))			Inputs->Configure();
		if(0 == strcmp(aName, "DoTextFile"))			TextFile->Do();
		if(0 == strcmp(aName, "DoExit"))				Exit();

		if(0 == strcmp(aName, "DoToggleRecordVideo"))
		{
			if(RecordingWave)
			{
				MDFND_DispMessage((UTF8*)"Can't record video and audio simultaneously.");		
			}
			else
			{
				if(RecordingVideo)
				{
					MDFND_DispMessage((UTF8*)"Finished recording video.");
					MDFNI_StopAVRecord();
					RecordingVideo = false;
				}
				else
				{
					if(MDFNI_StartAVRecord(MDFN_MakeFName(MDFNMKF_VIDEO, 0, 0).c_str(), 48000))
					{
						MDFND_DispMessage((UTF8*)"Begin recording video.");
						RecordingVideo = true;
					}
					else
					{
						MDFND_DispMessage((UTF8*)"Failed to begin recording video.");
					}
				}
			}
		}

		if(0 == strcmp(aName, "DoToggleRecordWave"))
		{
			if(RecordingVideo)
			{
				MDFND_DispMessage((UTF8*)"Can't record video and audio simultaneously.");
			}
			else
			{
				if(RecordingWave)
				{
					MDFND_DispMessage((UTF8*)"Finished recording audio.");
					MDFNI_StopWAVRecord();
					RecordingWave = false;
				}
				else
				{
					if(MDFNI_StartWAVRecord(MDFN_MakeFName(MDFNMKF_AUDIO, 0, 0).c_str(), 48000))
					{
						MDFND_DispMessage((UTF8*)"Begin recording audio.");
						RecordingWave = true;
					}
					else
					{
						MDFND_DispMessage((UTF8*)"Failed to begin recording audio.");
					}
				}
			}
		}

		
		Inputs->ReadSettings();		static uint32_t					SkipCount;
		
		delete Scaler;
		Scaler = 0;
	}
}

void						MednafenEmu::GenerateSettings	(std::vector<MDFNSetting>& aSettings)
{
	for(int i = 0; i != MDFNSystems.size(); i ++)
	{
		for(int j = 0; j != sizeof(SystemSettings) / sizeof(MDFNSetting); j++)
		{
			std::string myname = std::string(MDFNSystems[i]->shortname) + ".es." + std::string(SystemSettings[j].name);		
	
			MDFNSetting thisone;
			memcpy(&thisone, &SystemSettings[j], sizeof(MDFNSetting));
			//TODO: This strdup will not be freed
			thisone.name = strdup(myname.c_str());
			aSettings.push_back(thisone);
		}
	}
}

bool						MednafenEmu::IsInitialized = false;
bool						MednafenEmu::IsLoaded = false;
	
Texture*					MednafenEmu::Buffer = 0;
MDFN_Surface*				MednafenEmu::Surface = 0;

MDFNGI*						MednafenEmu::GameInfo = 0;
InputHandler*				MednafenEmu::Inputs = 0;
TextViewer*					MednafenEmu::TextFile = 0;
Filter*						MednafenEmu::Scaler = 0;
FastCounter					MednafenEmu::Counter;
EmuRealSyncher				MednafenEmu::Syncher;

std::string					MednafenEmu::Message;
uint32_t					MednafenEmu::MessageTime = 0;
bool						MednafenEmu::RewindEnabled = false;
bool						MednafenEmu::RecordingVideo = false;
bool						MednafenEmu::RecordingWave = false;

std::vector<MDFNSetting>	MednafenEmu::Settings;

EmulateSpecStruct			MednafenEmu::EmulatorSpec;	
MDFN_Rect					MednafenEmu::VideoWidths[512];
int16_t						MednafenEmu::Samples[48000];
int16_t						MednafenEmu::SamplesUp[48000];
bool						MednafenEmu::SkipNext = false;
uint32_t					MednafenEmu::SkipCount = 0;
