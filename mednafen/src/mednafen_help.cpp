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
		const char *subs[7] = {"mcs", "mcm", "snaps", "palettes", "sav", "cheats", "firmware"};

		if(mkdir(es_paths->Build("mednafen").c_str(), S_IRWXU) == -1 && errno != EEXIST)
		{
			return;
		}

		char buffer[1024];

		for(int i = 0; i != 7; i ++)
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
		{"autosave", MDFNSF_NOFLAGS, "Save state at exit", NULL, MDFNST_BOOL, "0"}
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
		for(int i = 0; i != sizeof(ESSettings) / sizeof(MDFNSetting); i++)
		{
			Settings.push_back(ESSettings[i]);
		}

		Buffer = es_video->CreateTexture(1920, 1080);

		std::vector<MDFNGI*> externalSystems;
#ifndef L1GHT
		externalSystems.push_back(GetPCSX());
#endif
		externalSystems.push_back(GetNestopia());
		externalSystems.push_back(GetGambatte());
#ifdef L1GHT
		externalSystems.push_back(vbamGetVBAM());
#endif
		MDFNI_InitializeModules(externalSystems);

		GenerateSettings(Settings);
		InputHandler::GenerateSettings(Settings);

		MakeDirectories();
		MDFNI_Initialize(es_paths->Build("mednafen").c_str(), Settings);
	}

	IsInitialized = true;
}

void						MednafenEmu::Quit				()
{
	if(IsInitialized)
	{
		delete Buffer;

		CloseGame();
		MDFNI_Kill();
	}

	IsInitialized = false;
}

void						MednafenEmu::LoadGame			(std::string aFileName, void* aData, int aSize)
{
	if(!IsLoaded && IsInitialized)
	{
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

		Surface = new MDFN_Surface(0, GameInfo->fb_width, GameInfo->fb_height, GameInfo->fb_width, MDFN_PixelFormat(MDFN_COLORSPACE_RGB, 16, 8, 0, 24));	
		Inputs = new InputHandler(GameInfo);
		TextFile = new TextViewer(aFileName + ".txt");


		if(MDFN_GetSettingB(SETTINGNAME("autosave")))
		{
			MDFNI_LoadState(0, "mcq");
		}
	
		PCESkipHack = strcmp(GameInfo->shortname, "pce") == 0 || strcmp(GameInfo->shortname, "pce_fast") == 0;
		
#if 0
		MDFND_NetStart();
#endif
		
		IsLoaded = true;
	}
}

void						MednafenEmu::CloseGame			()
{
	if(IsLoaded && IsInitialized)
	{
		if(MDFN_GetSettingB(SETTINGNAME("autosave")))
		{
			MDFNI_SaveState(0, "mcq", 0, 0, 0);
		}
	
		MDFNI_CloseGame();
		
		delete Inputs;
		delete Surface;
		delete TextFile;
		delete Scaler;
		
		GameInfo = 0;
		Surface = 0;
		Inputs = 0;
		TextFile = 0;
		Scaler = 0;
		
		IsLoaded = false;		
	}
}

void						MednafenEmu::Frame				()
{
	if(IsInitialized && IsLoaded)
	{
		Counter.Tick();
	
		Inputs->Process();
		static uint32_t srate = 48000;
		static bool sratelo = false;
	
		memset(VideoWidths, 0xFF, sizeof(MDFN_Rect) * 512);
		memset(&EmulatorSpec, 0, sizeof(EmulateSpecStruct));
		EmulatorSpec.surface = Surface;
		EmulatorSpec.LineWidths = VideoWidths;
		EmulatorSpec.soundmultiplier = 1;
		EmulatorSpec.SoundRate = srate;
		EmulatorSpec.SoundBuf = Samples;
		EmulatorSpec.SoundBufMaxSize = 24000;
		EmulatorSpec.SoundVolume = 1;
		EmulatorSpec.NeedRewind = es_input->ButtonPressed(0, ES_BUTTON_AUXLEFT2);
		EmulatorSpec.skip = !Counter.DrawNow() && !PCESkipHack;
		MDFNI_Emulate(&EmulatorSpec);
		
		if(Counter.DrawNow())
		{
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

			//AUDIO			
			if(!sratelo && EmulatorSpec.SoundBufSize < 799)
			{
				srate ++;
			}
			else if(!sratelo && EmulatorSpec.SoundBufSize > 801)
			{
				srate --;
			}
			else sratelo = true;
			
			if(GameInfo->soundchan > 1)
			{
				es_audio->AddSamples((uint32_t*)Samples, EmulatorSpec.SoundBufSize);
			}
			else
			{
				for(int i = 0; i != EmulatorSpec.SoundBufSize; i ++)
				{
					SamplesUp[i * 2] = Samples[i];
					SamplesUp[i * 2 + 1] = Samples[i];
				}
				es_audio->AddSamples((uint32_t*)SamplesUp, EmulatorSpec.SoundBufSize);			
			}
		}
		
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

void						MednafenEmu::DoCommand			(std::string aName)
{
	if(IsLoaded)
	{
		if(aName == "DoReload")					ReloadEmulator();
		if(aName == "DoSettings")				MednafenSettings(GameInfo->shortname).Do();
		if(aName == "DoReset")					MDFNI_Reset();
		if(aName == "DoScreenShot")				MDFNI_SaveSnapshot(Surface, &EmulatorSpec.DisplayRect, VideoWidths);
		if(aName == "DoSaveState")				MDFNI_SaveState(0, 0, Surface, &EmulatorSpec.DisplayRect, VideoWidths);
		if(aName == "DoLoadState")				MDFNI_LoadState(0, 0);
		if(aName == "DoStateMenu")				MednafenStateMenu().Do();
		if(aName == "DoToggleRewind")			MDFNI_EnableStateRewind(1);
		if(aName == "DoInputConfig")			Inputs->Configure();
		if(aName == "DoTextFile")				TextFile->Do();
		if(aName == "DoExit")					Exit();
		
		Inputs->ReadSettings();
		
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
InputHandler*				MednafenEmu::Inputs = 0;
TextViewer*					MednafenEmu::TextFile = 0;
FastCounter					MednafenEmu::Counter;
Filter*						MednafenEmu::Scaler = 0;

std::string					MednafenEmu::Message;
uint32_t					MednafenEmu::MessageTime = 0;
bool						MednafenEmu::PCESkipHack = false;

MDFNGI*						MednafenEmu::GameInfo = 0;

std::vector<MDFNSetting>	MednafenEmu::Settings;

EmulateSpecStruct			MednafenEmu::EmulatorSpec;	
MDFN_Rect					MednafenEmu::VideoWidths[512];
int16_t						MednafenEmu::Samples[48000];
int16_t						MednafenEmu::SamplesUp[48000];

