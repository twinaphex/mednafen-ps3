#include <mednafen_includes.h>

namespace
{
	#define SETTINGNAME(b) ((SettingHeader + b).c_str())

	MDFNSetting SystemSettings[] = 
	{
		{"underscan", MDFNSF_NOFLAGS, "Reduce size of screen to compensate for display overscan.", NULL, MDFNST_INT, "5", "0", "50" },
		{"displayfps", MDFNSF_NOFLAGS, "Display frames per second in corner of screen", NULL, MDFNST_BOOL, "0" },
		{"filter", MDFNSF_NOFLAGS, "Use bilinear filter for display", NULL, MDFNST_BOOL, "0"},
		{"fullframe", MDFNSF_NOFLAGS, "Ignore screen aspect ratio", NULL, MDFNST_BOOL, "0"},
		{"autosave", MDFNSF_NOFLAGS, "Save state at exit", NULL, MDFNST_BOOL, "0"}
	};

	MDFNSetting PS3Settings[] =
	{
		{"ps3.bookmarks", MDFNSF_NOFLAGS, "Bookmarks for the file browser.", NULL, MDFNST_STRING, "" },
	};
}



void						MednafenEmu::Init				()
{
	if(!IsInitialized)
	{
		for(int i = 0; i != sizeof(PS3Settings) / sizeof(MDFNSetting); i++)
		{
			Settings.push_back(PS3Settings[i]);
		}

		Buffer = new Texture(1920, 1080);

		std::vector<MDFNGI*> externalSystems;
		externalSystems.push_back(vbamGetVBAM());
		MDFNI_InitializeModules(externalSystems);

		GenerateSettings(Settings);
		InputHandler::GenerateSettings(Settings);

		MDFNI_Initialize(Paths.Build("mednafen").c_str(), Settings);
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

void						MednafenEmu::DisplayMessage		(std::string aMessage)
{
	if(IsInitialized && IsLoaded)
	{
		Message = aMessage;
		MessageTime = MDFND_GetTime();
	}
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
			ps3_log->Do();
			Exit();
		}
	
		Surface = new MDFN_Surface(0, GameInfo->fb_width, GameInfo->fb_height, GameInfo->fb_width, MDFN_PixelFormat(MDFN_COLORSPACE_RGB, 16, 8, 0, 24));	
		Inputs = new InputHandler(GameInfo);
		TextFile = new TextViewer(aFileName + ".txt");
	
		SettingHeader = std::string(GameInfo->shortname) + ".ps3.";
		
		if(MDFN_GetSettingB(SETTINGNAME("autosave")))
		{
			MDFNI_LoadState(0, "mcq");
		}
	
		PCESkipHack = strcmp(GameInfo->shortname, "pce") == 0 || strcmp(GameInfo->shortname, "pce_fast") == 0;
		
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
		
		GameInfo = 0;
		Surface = 0;
		Inputs = 0;
		TextFile = 0;
		
		IsLoaded = false;		
	}
}

void						MednafenEmu::Frame				()
{
	if(IsInitialized && IsLoaded)
	{
		Counter.Tick();
	
		Inputs->Process();
	
		memset(VideoWidths, 0xFF, sizeof(MDFN_Rect) * 512);
	
		EmulateSpecStruct espec;
		memset(&espec, 0, sizeof(EmulateSpecStruct));
		espec.surface = Surface;
		espec.LineWidths = VideoWidths;
		espec.soundmultiplier = 1;
		espec.SoundRate = 48000;
		espec.SoundBuf = Samples;
		espec.SoundBufMaxSize = 48000;
		espec.SoundVolume = 1;
		espec.NeedRewind = PS3Input::ButtonPressed(0, PS3_BUTTON_L2);
		espec.skip = !Counter.DrawNow() && !PCESkipHack;
		MDFNI_Emulate(&espec);
	
		DisplayRect = espec.DisplayRect;
		
		if(Counter.DrawNow())
		{
			Buffer->SetFilter(MDFN_GetSettingB(SETTINGNAME("filter")));
			
			if(GameInfo->soundchan > 1)
			{
				PS3Audio::AddSamples((uint32_t*)Samples, espec.SoundBufSize);
			}
			else
			{
				for(int i = 0; i != espec.SoundBufSize; i ++)
				{
					SamplesUp[i * 2] = Samples[i];
					SamplesUp[i * 2 + 1] = Samples[i];
				}
				PS3Audio::AddSamples((uint32_t*)SamplesUp, espec.SoundBufSize);			
			}
			
			uint32_t real_x = VideoWidths[0].w != ~0 ? VideoWidths[0].x : espec.DisplayRect.x;
			uint32_t real_width = VideoWidths[0].w != ~0 ? VideoWidths[0].w : espec.DisplayRect.w;
	
			uint32_t* pix = Buffer->GetPixels();
			uint32_t pixw = Buffer->GetWidth();
			for(int i = 0; i != espec.DisplayRect.h; i ++)
			{
				for(int j = 0; j != real_width; j ++)
				{
					pix[i * pixw + j] = Surface->pixels[(i + espec.DisplayRect.y) * Surface->pitchinpix + (j + real_x)] | 0xFF000000;
				}
			}
			
			PS3Video::PresentFrame(Buffer, Area(0, 0, real_width, espec.DisplayRect.h), MDFN_GetSettingB(SETTINGNAME("fullframe")), MDFN_GetSettingUI(SETTINGNAME("underscan")));
	
			if(MDFN_GetSettingB(SETTINGNAME("displayfps")))
			{
				char buffer[128];
				snprintf(buffer, 128, "%d", Counter.GetFPS());
				FontManager::GetBigFont()->PutString(buffer, 10, 10, 0xFFFFFFFF);
			}
				
			if(MDFND_GetTime() - MessageTime < 5000)
			{
				FontManager::GetBigFont()->PutString(Message, 10, 10 + FontManager::GetBigFont()->GetHeight(), 0xFFFFFFFF);
			}
	
			PS3Video::Flip();					
		}
		
		if(!PS3Input::ButtonPressed(0, PS3_BUTTON_L3) && PS3Input::ButtonDown(0, PS3_BUTTON_R3))
		{
			MednafenCommands().Do();
		}
	}
}

void						MednafenEmu::DummyFrame			()
{
	if(IsLoaded)
	{
//TODO: Change to present frame
//		PS3Video::PresentFrame(Buffer, Area(0, 0, real_width, espec.DisplayRect.h), false, 0);
	}
	
	PS3Video::Flip();					
}

void						MednafenEmu::DoCommand			(std::string aName)
{
	if(IsLoaded)
	{
		if(aName == "DoReload")					ReloadEmulator();
		if(aName == "DoSettings")				MednafenSettings(GameInfo->shortname).Do();
		if(aName == "DoReset")					MDFNI_Reset();
		if(aName == "DoScreenShot")				MDFNI_SaveSnapshot(Surface, &DisplayRect, VideoWidths);
		if(aName == "DoSaveState")				MDFNI_SaveState(0, 0, 0, 0, 0);
		if(aName == "DoLoadState")				MDFNI_LoadState(0, 0);
		if(aName == "DoToggleRewind")			MDFNI_EnableStateRewind(1);
		if(aName == "DoInputConfig")			Inputs->Configure();
		if(aName == "DoTextFile")				TextFile->Do();
		if(aName == "DoExit")					Exit();
		
		Inputs->ReadSettings();
	}
}

void						MednafenEmu::GenerateSettings	(std::vector<MDFNSetting>& aSettings)
{
	for(int i = 0; i != MDFNSystems.size(); i ++)
	{
		for(int j = 0; j != sizeof(SystemSettings) / sizeof(MDFNSetting); j++)
		{
			std::string myname = std::string(MDFNSystems[i]->shortname) + ".ps3." + std::string(SystemSettings[j].name);		
	
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
	
std::string					MednafenEmu::Message;
uint32_t					MednafenEmu::MessageTime = 0;
bool						MednafenEmu::PCESkipHack = false;
	
MDFNGI*						MednafenEmu::GameInfo = 0;

std::vector<MDFNSetting>	MednafenEmu::Settings;
std::string					MednafenEmu::SettingHeader;
	
MDFN_Rect					MednafenEmu::VideoWidths[512];
MDFN_Rect					MednafenEmu::DisplayRect;
		
int16						MednafenEmu::Samples[2*48000];
int16_t						MednafenEmu::SamplesUp[48000];

