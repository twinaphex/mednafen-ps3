#include <mednafen_includes.h>
#include "savestates.h"

namespace
{
	uint32_t		CurrentFilter = 0;
	Filter*			BuildFilter			(uint32_t aIndex)
	{
		CurrentFilter = aIndex;

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

#ifdef __WIN32__
		if(mkdir(es_paths->Build("mednafen").c_str()) == -1 && errno != EEXIST)
#else
		if(mkdir(es_paths->Build("mednafen").c_str(), S_IRWXU) == -1 && errno != EEXIST)
#endif
		{
			return;
		}

		char buffer[1024];

		for(int i = 0; i != 9; i ++)
		{
			sprintf(buffer, "mednafen"PSS"%s", subs[i]);
#ifdef __WIN32__
			if(mkdir(es_paths->Build(buffer).c_str()) == -1 && errno != EEXIST)
#else
			if(mkdir(es_paths->Build(buffer).c_str(), S_IRWXU) == -1 && errno != EEXIST)
#endif
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
		{"underscan", MDFNSF_NOFLAGS, "Reduce size of screen to compensate for display overscan.", NULL, MDFNST_INT, "5", "-50", "50" },
		{"undertunetop", MDFNSF_NOFLAGS, "Fine tune underscan at top of screen.", NULL, MDFNST_INT, "0", "-50", "50" },
		{"undertunebottom", MDFNSF_NOFLAGS, "Fine tune underscan at bottom of screen.", NULL, MDFNST_INT, "0", "-50", "50" },
		{"undertuneleft", MDFNSF_NOFLAGS, "Fine tune underscan at left of screen.", NULL, MDFNST_INT, "0", "-50", "50" },
		{"undertuneright", MDFNSF_NOFLAGS, "Fine tune underscan at right of screen.", NULL, MDFNST_INT, "0", "-50", "50" },
		{"displayfps", MDFNSF_NOFLAGS, "Display frames per second in corner of screen", NULL, MDFNST_BOOL, "0" },
		{"filter", MDFNSF_NOFLAGS, "Use bilinear filter for display", NULL, MDFNST_BOOL, "0"},
		{"fullframe", MDFNSF_NOFLAGS, "Ignore screen aspect ratio", NULL, MDFNST_BOOL, "0"},
		{"autosave", MDFNSF_NOFLAGS, "Save state at exit", NULL, MDFNST_BOOL, "0"},
		{"rewind", MDFNSF_NOFLAGS, "Enable Rewind Support", NULL, MDFNST_BOOL, "0"}
	};

	MDFNSetting ESSettings[] =
	{
		{"es.bookmarks", MDFNSF_NOFLAGS, "Bookmarks for the file browser.", NULL, MDFNST_STRING, "" },
		{"net.es.username", MDFNSF_NOFLAGS, "User name for netplay.", NULL, MDFNST_STRING, "Me" },		
		{"net.es.password", MDFNSF_NOFLAGS, "Password for the netplay Server.", NULL, MDFNST_STRING, "sexybeef" },
		{"net.es.host", MDFNSF_NOFLAGS, "Hostname for netplay Server.", NULL, MDFNST_STRING, "192.168.0.115" },
		{"net.es.port", MDFNSF_NOFLAGS, "Port for netplay Server.", NULL, MDFNST_UINT, "4046" },
		{"net.es.gameid", MDFNSF_NOFLAGS, "Game ID for netplay Server.", NULL, MDFNST_STRING, "doing" },
	};
}

//TODO: Find this a new home
extern bool					NetplayOn;

void						MednafenEmu::Init				()
{
	if(!IsInitialized)
	{
		//Put settings for the user interface
		for(int i = 0; i != sizeof(ESSettings) / sizeof(MDFNSetting); i++)
		{
			Settings.push_back(ESSettings[i]);
		}

		//Get the external emulators
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
//HACK:
		Buffer = es_video->CreateTexture(640, 480, false);
		bool slowread = Buffer->GetFlags() & Texture::SLOW_READ;
		Surface = new MDFN_Surface(slowread ? 0 : Buffer->GetPixels(), 640, 480, slowread ? 480 : Buffer->GetPitch(), MDFN_PixelFormat(MDFN_COLORSPACE_RGB, 16, 8, 0, 24));

//		Buffer = es_video->CreateTexture(1920, 1080);
//		Surface = new MDFN_Surface(0, 1920, 1080, 1920, MDFN_PixelFormat(MDFN_COLORSPACE_RGB, 16, 8, 0, 24));
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
			Summerface("Log", es_log).Do();
			Exit();
		}

		MDFND_NetworkClose();
		MDFNI_EnableStateRewind(false);
		RewindEnabled = false;

		Buffer->Clear(0);

		Inputs = new InputHandler(GameInfo);

		if(MDFN_GetSettingB(SETTINGNAME("autosave")))
		{
			MDFNI_LoadState(0, "mcq");
		}
	
		IsLoaded = true;
		SkipCount = 0;
		SkipNext = false;

		MDFND_DispMessage((UTF8*)GameInfo->fullname);

		Syncher.SetEmuClock(GameInfo->MasterClock >> 32);

		SuspendDraw = false;

		ReadSettings();
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
		MDFND_Rumble(0, 0);
		
		delete Inputs;
		delete Scaler;
		
		GameInfo = 0;
		Inputs = 0;
		Scaler = 0;
		
		IsLoaded = false;
	}
}

bool						MednafenEmu::Frame				()
{
	if(IsInitialized && IsLoaded)
	{
		if(RewindSetting != RewindEnabled)
		{
			MDFNI_EnableStateRewind(RewindSetting);
			RewindEnabled = RewindSetting;
		}

		Inputs->Process();
	
		if(NetplayOn)
		{
			Syncher.Sync();
		}

		memset(VideoWidths, 0xFF, sizeof(MDFN_Rect) * 512);
		memset(&EmulatorSpec, 0, sizeof(EmulateSpecStruct));
		EmulatorSpec.surface = Surface;
		EmulatorSpec.LineWidths = VideoWidths;
		EmulatorSpec.soundmultiplier = NetplayOn ? 1 : Counter.GetSpeed();
		EmulatorSpec.SoundRate = 48000;
		EmulatorSpec.SoundBuf = Samples;
		EmulatorSpec.SoundBufMaxSize = 24000;
		EmulatorSpec.SoundVolume = 1;
		EmulatorSpec.NeedRewind = !NetplayOn && es_input->ButtonPressed(0, ES_BUTTON_AUXLEFT2);
		EmulatorSpec.skip = NetplayOn ? Syncher.NeedFrameSkip() : (SkipNext && ((SkipCount ++) < 4));
		MDFNI_Emulate(&EmulatorSpec);

		Syncher.AddEmuTime(EmulatorSpec.MasterCycles / (NetplayOn ? 1 : Counter.GetSpeed()));
		Counter.Tick(EmulatorSpec.skip);

		//VIDEO
		if(!EmulatorSpec.skip)
		{
			SkipCount = 0;

			Blit();
	
			if(DisplayFPSSetting)
			{
				char buffer[128];
				uint32_t fps, skip;
				fps = Counter.GetFPS(&skip);
				snprintf(buffer, 128, "%d (%d)", fps, skip);
				FontManager::GetBigFont()->PutString(buffer, 10, 10, 0xFFFFFFFF);
			}
				
			if(MDFND_GetTime() - MessageTime < 5000)
			{
				FontManager::GetBigFont()->PutString(Message.c_str(), 10, 10 + FontManager::GetBigFont()->GetHeight(), 0xFFFFFFFF);
			}
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


		if(NetplayOn && es_input->ButtonDown(0, ES_BUTTON_AUXRIGHT3) && es_input->ButtonPressed(0, ES_BUTTON_AUXRIGHT2))
		{
			MDFND_NetworkClose();
		}
		else if(es_input->ButtonDown(0, ES_BUTTON_AUXRIGHT3))
		{
			DoCommands();
			return false;
		}

		return !EmulatorSpec.skip;
	}

	return false;
}

void						MednafenEmu::Blit				(uint32_t* aPixels, uint32_t aWidth, uint32_t aHeight, uint32_t aPitch, bool aDummy)
{
	//Get the output area
	Area output(VideoWidths[0].w != ~0 ? VideoWidths[0].x : EmulatorSpec.DisplayRect.x, EmulatorSpec.DisplayRect.y, VideoWidths[0].w != ~0 ? VideoWidths[0].w : EmulatorSpec.DisplayRect.w, EmulatorSpec.DisplayRect.h);
	if(aPixels)
	{
		output = Area(0, 0, aWidth, aHeight);
	}

	//If we are drawing predefined pixels, or have a slow_read texture, move it into the buffer texture
	if(aPixels || (Buffer->GetFlags() & Texture::SLOW_READ))
	{
		uint32_t* pix = Buffer->GetPixels();
		uint32_t* srcpix = aPixels ? aPixels : Surface->pixels;
		uint32_t srcpitch = aPixels ? aPitch : Surface->pitchinpix;

		for(int i = 0; i != aHeight; i ++)
		{
			memcpy(&pix[i * Buffer->GetPitch()], &srcpix[i * srcpitch], aWidth * 4);
		}
	}

	//Invalidate and draw the buffer
	if(!aDummy)
	{
		Buffer->Invalidate();
	}

	es_video->PresentFrame(Buffer, output, FullFrameSetting, UnderscanSetting, UndertuneSetting);
	
/*
	if(IsInitialized && (aPixels || IsLoaded))
	{
		Area output(VideoWidths[0].w != ~0 ? VideoWidths[0].x : EmulatorSpec.DisplayRect.x, EmulatorSpec.DisplayRect.y, VideoWidths[0].w != ~0 ? VideoWidths[0].w : EmulatorSpec.DisplayRect.w, EmulatorSpec.DisplayRect.h);

		if(aPixels)
		{
			output = Area(0, 0, aWidth, aHeight);
		}

		if(Scaler && CurrentFilter != MDFN_GetSettingUI(SETTINGNAME("scaler")))
		{
			delete Scaler;
			Scaler = 0;
		}

		Buffer->SetFilter(MDFN_GetSettingB(SETTINGNAME("filter")));

		if(!Scaler)
		{
			Scaler = BuildFilter(MDFN_GetSettingUI(SETTINGNAME("scaler")));
			Scaler->init(output.Width, output.Height);
		}

		if(output.Width != Scaler->getWidth() || output.Height != Scaler->getHeight())
		{
			Scaler->init(output.Width, output.Height);
		}

		uint32_t* scaleIn = Scaler->inBuffer();
		uint32_t scaleInP = Scaler->inPitch();

		uint32_t* scaleOut = Scaler->outBuffer();
		uint32_t scaleOutP = Scaler->outPitch();

		uint32_t* bufferPix = Buffer->GetPixels();
		uint32_t bufferP = Buffer->GetWidth();

		uint32_t finalWidth = output.Width * Scaler->info().outWidth;
		uint32_t finalHeight = output.Height * Scaler->info().outHeight;

		uint32_t* pixels = aPixels ? aPixels : Surface->pixels;
		uint32_t pitch = aPixels ? aPitch : Surface->pitchinpix;

		for(int i = 0; i != output.Height; i ++)
		{
			memcpy(&scaleIn[i * scaleInP], &pixels[(output.Y + i) * pitch + output.X], output.Width * 4);
		}

		Scaler->filter();
	
		for(int i = 0; i != finalHeight; i ++)
		{
			memcpy(&bufferPix[i * bufferP], &scaleOut[i * scaleOutP], finalWidth * 4);	
		}

		Area underscanfine();

		es_video->PresentFrame(Buffer, Area(0, 0, finalWidth, finalHeight), MDFN_GetSettingB(SETTINGNAME("fullframe")), MDFN_GetSettingI(SETTINGNAME("underscan")), underscanfine);
	}*/
}

void						MednafenEmu::DoCommands			()
{
	MDFND_Rumble(0, 0);

	const char*	commands[] =
	{
		//Display name,			Image name,			Command name
		"Change Game",			"DoReload",			"DoReload",
		"Reset Game",			"DoReset",			"DoReset",
		"Show Text File",		"DoTextFile",		"DoTextFile",
		"Connect Netplay",		"DoNetplay",		"DoNetplay",
		"Save State",			"DoSaveState",		"DoSaveStateMenu",
		"Load State",			"DoLoadState",		"DoLoadStateMenu",
		"Take Screen Shot",		"DoScreenShot",		"DoScreenShot",
		"Change Disk Side",		"DoDiskSide", 		"DoDiskSide",
		"Settings",				"DoSettings",		"DoSettings",
		"Configure Controls",	"DoInputConfig",	"DoInputConfig",
		"Record Video",			"DoRecordVideo",	"DoToggleRecordVideo",
		"Record Audio",			"DoRecordAudio",	"DoToggleRecordWave",
	};

	SummerfaceList* grid = new SummerfaceList(Area(25, 25, 50, 50));
	grid->SetView(new GridListView(grid, 4, 3, true, false));
	grid->SetHeader("Choose Action");
	grid->SetInputConduit(new SummerfaceStaticConduit(DoCommand, 0), true);
	for(int i = 0; i != 12; i ++)
	{
		SummerfaceItem* item = new SummerfaceItem(commands[i * 3], commands[i * 3 + 1]);
		item->Properties["COMMAND"] = commands[i * 3 + 2];
		grid->AddItem(item);
	}

	Summerface sface("Grid", grid); sface.Do();
}

bool						MednafenEmu::DoCommand			(void* aUserData, Summerface* aInterface, const std::string& aWindow)
{
	std::string command;

	if(aInterface && aInterface->GetWindow(aWindow) && es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
	{
		command = ((SummerfaceList*)aInterface->GetWindow(aWindow))->GetSelected()->Properties["COMMAND"];
	}
	else if(!aInterface)
	{
		command = aWindow;
	}
	else
	{
		return false;
	}

	if(IsLoaded)
	{
		if(0 == strcmp(command.c_str(), "DoDiskSide"))			MDFN_DoSimpleCommand(MDFN_MSC_SELECT_DISK);
		if(0 == strcmp(command.c_str(), "DoReload"))			ReloadEmulator();
		if(0 == strcmp(command.c_str(), "DoSettings"))			{MednafenSettings::Do(GameInfo->shortname); ReadSettings();}
		if(0 == strcmp(command.c_str(), "DoReset"))				MDFNI_Reset();
		if(0 == strcmp(command.c_str(), "DoNetplay"))			MDFND_NetStart();
		if(0 == strcmp(command.c_str(), "DoScreenShot"))		MDFNI_SaveSnapshot(Surface, &EmulatorSpec.DisplayRect, VideoWidths);
		if(0 == strcmp(command.c_str(), "DoSaveState"))			MDFNI_SaveState(0, 0, Surface, &EmulatorSpec.DisplayRect, VideoWidths);
		if(0 == strcmp(command.c_str(), "DoLoadState"))			MDFNI_LoadState(0, 0);
		if(0 == strcmp(command.c_str(), "DoSaveStateMenu"))		{SuspendDraw = true; StateMenu(false).Do(); SuspendDraw = false;}
		if(0 == strcmp(command.c_str(), "DoLoadStateMenu"))		{SuspendDraw = true; StateMenu(true).Do(); SuspendDraw = false;}
		if(0 == strcmp(command.c_str(), "DoInputConfig"))		Inputs->Configure();
		if(0 == strcmp(command.c_str(), "DoExit"))				Exit();

		if(0 == strcmp(command.c_str(), "DoTextFile"))
		{
			std::vector<std::string> nbm;
			FileSelect FileChooser("Select Text File", nbm, "");
			std::string filename = FileChooser.GetFile();

			if(!filename.empty())
			{
				TextViewer* tv = new TextViewer(Area(10, 10, 80, 80), filename);
				tv->SetHeader(filename);
				Summerface("TextView", tv).Do();
			}
		}

		if(0 == strcmp(command.c_str(), "DoToggleRecordVideo"))
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

		if(0 == strcmp(command.c_str(), "DoToggleRecordWave"))
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

		
		Inputs->ReadSettings();
		
		delete Scaler;
		Scaler = 0;

		return true;
	}

	return false;
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

void						MednafenEmu::ReadSettings		()
{
	RewindSetting = MDFN_GetSettingB(SETTINGNAME("rewind"));;
	DisplayFPSSetting = MDFN_GetSettingB(SETTINGNAME("displayfps"));
	FullFrameSetting = MDFN_GetSettingB(SETTINGNAME("fullframe"));
	UnderscanSetting = MDFN_GetSettingI(SETTINGNAME("underscan"));
	FilterSetting = MDFN_GetSettingB(SETTINGNAME("filter"));
	ScalerSetting = MDFN_GetSettingI(SETTINGNAME("scaler"));
	UndertuneSetting = Area(MDFN_GetSettingI(SETTINGNAME("undertuneleft")), MDFN_GetSettingI(SETTINGNAME("undertunetop")), MDFN_GetSettingI(SETTINGNAME("undertuneright")), MDFN_GetSettingI(SETTINGNAME("undertunebottom")));
}

bool						MednafenEmu::IsInitialized = false;
bool						MednafenEmu::IsLoaded = false;
	
Texture*					MednafenEmu::Buffer = 0;
MDFN_Surface*				MednafenEmu::Surface = 0;
bool						MednafenEmu::SuspendDraw = false;

MDFNGI*						MednafenEmu::GameInfo = 0;
InputHandler*				MednafenEmu::Inputs = 0;
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

bool						MednafenEmu::RewindSetting = false;
bool						MednafenEmu::DisplayFPSSetting = false;
bool						MednafenEmu::FullFrameSetting = false;
int32_t						MednafenEmu::UnderscanSetting = 10;
bool						MednafenEmu::FilterSetting = false;
int32_t						MednafenEmu::ScalerSetting = 0;
Area						MednafenEmu::UndertuneSetting = Area(0, 0, 0, 0);
