#include <mednafen_includes.h>
#include "savestates.h"

//HACK
extern "C"
{
	#include <src/lua/lua.h>
	#include <src/lua/lauxlib.h>
	#include <src/lua/lualib.h>
}

extern luaL_reg emulib[];
extern luaL_reg romlib[];
extern luaL_reg joypadlib[];
extern luaL_reg guilib[];
extern luaL_reg memorylib[];
extern uint32_t gui_array[1024 * 768];

namespace
{
	const MDFNSetting_EnumList	AspectEnumList[] =
	{
		{"auto", 0, "Autodetect (Broken)", ""},
		{"pillarbox", -1, "Pillarbox", ""},
		{"fullframe", 1, "No Correction", ""},
		{0, 0, 0, 0}
	};

	MDFNSetting_EnumList*		BuildShaderEnum					()
	{
		static MDFNSetting_EnumList* results = 0;
		int onresult = 1;

		if(results == 0)
		{
			std::vector<std::string> shaders;
			if(Utility::ListDirectory(es_paths->Build("assets/presets"), shaders))
			{
				std::sort(shaders.begin(), shaders.end());

				results = new MDFNSetting_EnumList[shaders.size() + 2];
				results[0].string = "Standard";
				results[0].number = -1;
				results[0].description = "Standard";
				results[0].description_extra = "";

				for(std::vector<std::string>::iterator i = shaders.begin(); i != shaders.end(); i ++)
				{
					results[onresult].string = strdup(i->c_str());
					results[onresult].number = onresult;
					results[onresult].description = results[onresult].string;
					results[onresult++].description_extra = 0;
				}

				memset(&results[onresult], 0, sizeof(results[0]));
			}
			else
			{
				results = new MDFNSetting_EnumList[2];
				results[0].string = "Standard";
				results[0].number = -1;
				results[0].description = "Standard";
				results[0].description_extra = "";
				memset(&results[1], 0, sizeof(results[1]));
			}
		}

		return results;
	}


	#define SETTINGNAME(b) ((std::string(GameInfo->shortname) + ".es." + b).c_str())

	MDFNSetting SystemSettings[] = 
	{
		{"underscan", MDFNSF_NOFLAGS, "Reduce size of screen to compensate for display overscan.", NULL, MDFNST_INT, "5", "-50", "50" },
		{"undertunetop", MDFNSF_NOFLAGS, "Fine tune underscan at top of screen.", NULL, MDFNST_INT, "0", "-50", "50" },
		{"undertunebottom", MDFNSF_NOFLAGS, "Fine tune underscan at bottom of screen.", NULL, MDFNST_INT, "0", "-50", "50" },
		{"undertuneleft", MDFNSF_NOFLAGS, "Fine tune underscan at left of screen.", NULL, MDFNST_INT, "0", "-50", "50" },
		{"undertuneright", MDFNSF_NOFLAGS, "Fine tune underscan at right of screen.", NULL, MDFNST_INT, "0", "-50", "50" },
		{"display.fps", MDFNSF_NOFLAGS, "Display frames per second in corner of screen", NULL, MDFNST_BOOL, "0" },
		{"display.vsync", MDFNSF_NOFLAGS, "Enable vsync to prevent screen tearing.", NULL, MDFNST_BOOL, "1" },
		{"shader.preset", MDFNSF_NOFLAGS, "Shader preset for presenting the display", NULL, MDFNST_ENUM, "Standard", 0, 0, 0, 0, 0 },
		{"shader.prescale", MDFNSF_NOFLAGS, "Integer scale factor to apply before passing to shader", NULL, MDFNST_INT, "1", "1", "4" },
		{"shader.border", MDFNSF_NOFLAGS, "Path to Border to use with appropriate shaders.", NULL, MDFNST_STRING, "" },
		{"aspect", MDFNSF_NOFLAGS, "Override screen aspect correction", NULL, MDFNST_ENUM, "auto", NULL, NULL, NULL, NULL, AspectEnumList },
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
//		externalSystems.push_back(nestGetEmulator());
//		externalSystems.push_back(gmbtGetEmulator());
//		externalSystems.push_back(vbamGetEmulator());
//		externalSystems.push_back(pcsxGetEmulator());
		MDFNI_InitializeModules(externalSystems);

		//Make settings for each system
		GenerateSettings(Settings);
		InputHandler::GenerateSettings(Settings);

		//TODO: Build directory trees, if these don't exist we can't save games

		//Initialize mednafen and go
		MDFNI_Initialize(es_paths->Build("mednafen").c_str(), Settings);
	}

	IsInitialized = true;
}

void						MednafenEmu::Quit				()
{
	if(IsInitialized)
	{
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

		//Load the game
		//TODO: Support other casing of '.cue'
		if(strstr(aFileName.c_str(), ".cue"))
		{
			GameInfo = MDFNI_LoadCD(0, aFileName.c_str());
		}
		else
		{
			GameInfo = MDFNI_LoadGame(0, aFileName.c_str(), aData, aSize);
		}

		//Display log on error!
		if(!GameInfo)
		{
			Summerface::Create("Log", es_log)->Do();
			Exit();
		}

		ROMData = (uint8_t*)aData;
		ROMSize = aSize;

		//HACK: Attach a default border
		es_video->AttachBorder(ImageManager::GetImage("GameBorder"));

		Inputs = boost::make_shared<InputHandler>(GameInfo);

		//Reset states
		MDFND_NetworkClose();
		SkipCount = 0;
		FrameCount = 0;
		SkipNext = false;
		IsLoaded = true;
		IsPaused = false;
		SuspendDraw = false;
		RecordingVideo = false;
		RecordingWave = false;

		Syncher.SetEmuClock(GameInfo->MasterClock >> 32);

		ReadSettings(true);

		//Create the helpers for this game
		Buffer = Texture_Ptr(es_video->CreateTexture(GameInfo->fb_width, GameInfo->fb_height));
		Surface = boost::make_shared<MDFN_Surface>(Buffer->GetPixels(), GameInfo->fb_width, GameInfo->fb_height, GameInfo->fb_width, MDFN_PixelFormat(MDFN_COLORSPACE_RGB, Buffer->GetRedShift(), Buffer->GetGreenShift(), Buffer->GetBlueShift(), Buffer->GetAlphaShift()));

		//HACK: Setup lua
		std::string luafile = es_paths->Build(std::string("assets/lua/") + Utility::GetFileName(aFileName) + ".lua");
		if(Utility::FileExists(luafile))
		{
			Lua = new LuaScripter();
			Lua->RegisterLibrary("emu", emulib);
			Lua->RegisterLibrary("rom", romlib);
			Lua->RegisterLibrary("joypad", joypadlib);
			Lua->RegisterLibrary("gui", guilib);
			Lua->RegisterLibrary("memory", memorylib);

			lua_State *thread = lua_newthread(Lua->LuaState);
			Lua->LoadScript(luafile);
			lua_xmove(Lua->LuaState, thread, 1);
			lua_setfield(Lua->LuaState, LUA_REGISTRYINDEX, "emu.FrameAdvance");
		}
		else
		{
			Lua = 0;
		}

		//Load automatic state
		if(MDFN_GetSettingB(SETTINGNAME("autosave")))
		{
			MDFNI_LoadState(0, "mcq");
		}

		//Display emulator name	
		MDFND_DispMessage((UTF8*)GameInfo->fullname);
	}
}

void						MednafenEmu::CloseGame			()
{
	if(IsLoaded && IsInitialized)
	{
		//Stop any recordings
		if(RecordingVideo)
		{
			MDFNI_StopAVRecord();
		}

		if(RecordingWave)
		{
			MDFNI_StopWAVRecord();
		}

		//You know, I don't even remember what this is supposed to do...
		MDFNDES_BlockExit(true);

		//Save any anto states
		if(MDFN_GetSettingB(SETTINGNAME("autosave")))
		{
			MDFNI_SaveState(0, "mcq", 0, 0, 0);
		}
	
		//Close the game
		MDFNI_CloseGame();

		//Clean up
		MDFND_Rumble(0, 0);
		
		Inputs.reset();
		Buffer.reset();
		Surface.reset();

		delete Lua;

		free(ROMData);
		ROMData = 0;
		ROMSize = 0;

		IsLoaded = false;
	}
}

bool						MednafenEmu::Frame				()
{
	if(IsInitialized && IsLoaded)
	{
		if(!IsPaused)
		{
			MDFNI_EnableStateRewind(RewindSetting);

			Inputs->Process();
	
			if(NetplayOn)
			{
				Syncher.Sync();
			}

			//Emulate frame
			memset(VideoWidths, 0xFF, sizeof(MDFN_Rect) * 512);
			memset(&EmulatorSpec, 0, sizeof(EmulateSpecStruct));
			EmulatorSpec.surface = Surface.get(); //It's a shared pointer now
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
			FrameCount ++;

//HACK
			if(Lua)
			{
				lua_settop(Lua->LuaState, 0);
				lua_getfield(Lua->LuaState, LUA_REGISTRYINDEX, "emu.FrameAdvance");
				lua_State *thread = lua_tothread(Lua->LuaState, 1);
				if(LUA_YIELD != lua_resume(thread, 0))
				{
					char buffer[1024];
					snprintf(buffer, 1024, "Lua Error: %s", lua_tostring(thread, -1));
					MDFND_DispMessage((UTF8*)buffer);
					delete Lua;
					Lua = 0;
				}
			}

			//Handle inputs
			if(NetplayOn && es_input->ButtonDown(0, ES_BUTTON_AUXRIGHT3) && es_input->ButtonPressed(0, ES_BUTTON_AUXRIGHT2))
			{
				MDFND_NetworkClose();
			}
			else if(es_input->ButtonDown(0, ES_BUTTON_AUXRIGHT3))
			{
				DoCommands();
				return false;
			}
			else if(es_input->ButtonDown(0, ES_BUTTON_AUXLEFT3))
			{
				SetPause(true);
			}
			else
			{
				//VIDEO
				if(!EmulatorSpec.skip)
				{
					SkipCount = 0;
					Blit();

					//Show FPS
					if(DisplayFPSSetting)
					{
						char buffer[128];
						uint32_t fps, skip;
						fps = Counter.GetFPS(&skip);
						snprintf(buffer, 128, "%d (%d)", fps, skip);
						FontManager::GetBigFont()->PutString(buffer, 10, 10, 0xFFFFFFFF);
					}

					//Show message
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
			}

			return !EmulatorSpec.skip;
		}
		else
		{
			Inputs->Process();

//HACK:
			if(Lua)
			{
				lua_settop(Lua->LuaState, 0);
				lua_getfield(Lua->LuaState, LUA_REGISTRYINDEX, "emu.FrameAdvance");
				lua_State *thread = lua_tothread(Lua->LuaState, 1);
				if(LUA_YIELD != lua_resume(thread, 0))
				{
					char buffer[1024];
					snprintf(buffer, 1024, "Lua Error: %s", lua_tostring(thread, -1));
					MDFND_DispMessage((UTF8*)buffer);
					delete Lua;
					Lua = 0;
				}
			}

			if(NetplayOn && es_input->ButtonDown(0, ES_BUTTON_AUXRIGHT3) && es_input->ButtonPressed(0, ES_BUTTON_AUXRIGHT2))
			{
				MDFND_NetworkClose();
			}
			else if(es_input->ButtonDown(0, ES_BUTTON_AUXRIGHT3))
			{
				DoCommands();
				return false;
			}
			else if(es_input->ButtonDown(0, ES_BUTTON_AUXLEFT3))
			{
				SetPause(false);
			}

			Blit();
			//Show message
			if(MDFND_GetTime() - MessageTime < 5000)
			{
				FontManager::GetBigFont()->PutString(Message.c_str(), 10, 10 + FontManager::GetBigFont()->GetHeight(), 0xFFFFFFFF);
			}

			return true;
		}
	}

	return false;
}

void						MednafenEmu::Blit				(uint32_t* aPixels, uint32_t aWidth, uint32_t aHeight, uint32_t aPitch, bool aDummy)
{
	//HACK: Don't draw pixel arrays for now
	if(aPixels) return;

	if(IsInitialized && (aPixels || IsLoaded))
	{
		//Get the output area
		Area output(VideoWidths[0].w != ~0 ? VideoWidths[0].x : EmulatorSpec.DisplayRect.x, EmulatorSpec.DisplayRect.y, VideoWidths[0].w != ~0 ? VideoWidths[0].w : EmulatorSpec.DisplayRect.w, EmulatorSpec.DisplayRect.h);
		Buffer->Invalidate();
		es_video->PresentFrame(Buffer.get(), output, AspectSetting, UnderscanSetting, UndertuneSetting);

		if(Lua)
		{
			static Texture_Ptr tex(es_video->CreateTexture(1024, 768));
			memcpy(tex->GetPixels(), gui_array, 1024 * 768 * 4);
			memset(gui_array, 0, sizeof(gui_array));
			es_video->PlaceTexture(tex.get(), Area(0, 0, es_video->GetScreenWidth(), es_video->GetScreenHeight()), Area(0, 0, 1024, 768), 0xFFFFFFFF);
		}
	}
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

	SummerfaceList_Ptr grid = boost::make_shared<SummerfaceList>(Area(25, 25, 50, 50));
	grid->SetView(boost::make_shared<GridListView>(grid, 4, 3, true, false));
	grid->SetHeader("Choose Action");
	grid->SetInputConduit(boost::make_shared<SummerfaceStaticConduit>(DoCommand, (void*)0));
	for(int i = 0; i != 12; i ++)
	{
		SummerfaceItem_Ptr item = boost::make_shared<SummerfaceItem>(commands[i * 3], commands[i * 3 + 1]);
		item->Properties["COMMAND"] = commands[i * 3 + 2];
		grid->AddItem(item);
	}

	Summerface::Create("Grid", grid)->Do();
}

bool						MednafenEmu::DoCommand			(void* aUserData, Summerface_Ptr aInterface, const std::string& aWindow)
{
	std::string command;

	if(aInterface && aInterface->GetWindow(aWindow) && es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
	{
		SummerfaceList_Ptr list = boost::static_pointer_cast<SummerfaceList>(aInterface->GetWindow(aWindow));
		command = list->GetSelected()->Properties["COMMAND"];
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
		if(0 == strcmp(command.c_str(), "DoReload"))			ReloadEmulator("");
		if(0 == strcmp(command.c_str(), "DoSettings"))			{SettingMenu(GameInfo->shortname).Do();}
		if(0 == strcmp(command.c_str(), "DoReset"))				MDFNI_Reset();
		if(0 == strcmp(command.c_str(), "DoNetplay"))			MDFND_NetStart();
		if(0 == strcmp(command.c_str(), "DoScreenShot"))		MDFNI_SaveSnapshot(Surface.get(), &EmulatorSpec.DisplayRect, VideoWidths);
		if(0 == strcmp(command.c_str(), "DoSaveState"))			MDFNI_SaveState(0, 0, Surface.get(), &EmulatorSpec.DisplayRect, VideoWidths);
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
				boost::shared_ptr<TextViewer> tv = boost::make_shared<TextViewer>(Area(10, 10, 80, 80), filename);
				tv->SetHeader(filename);
				Summerface::Create("TextView", tv)->Do();
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
		return true;
	}

	return false;
}

void						MednafenEmu::SetPause			(bool aPause)
{
	IsPaused = aPause;

	if(IsPaused)
	{
		MDFND_DispMessage((UTF8*)"Game Paused");
	}
	else
	{
		MDFND_DispMessage((UTF8*)"Game Unpaused");
	}
}

uint32_t					MednafenEmu::GetPixel			(uint32_t aX, uint32_t aY)
{
	Area output(VideoWidths[0].w != ~0 ? VideoWidths[0].x : EmulatorSpec.DisplayRect.x, EmulatorSpec.DisplayRect.y, VideoWidths[0].w != ~0 ? VideoWidths[0].w : EmulatorSpec.DisplayRect.w, EmulatorSpec.DisplayRect.h);
	if(aX < output.Width && aY < output.Height)
	{
		return Surface->pixels[aY * Surface->pitchinpix + aX];
	}

	//?
	return 0xFF00FF00;
}

void						MednafenEmu::ReadSettings		(bool aOnLoad)
{
	if(IsGameLoaded())
	{
		RewindSetting = MDFN_GetSettingB(SETTINGNAME("rewind"));;
		DisplayFPSSetting = MDFN_GetSettingB(SETTINGNAME("display.fps"));
		AspectSetting = MDFN_GetSettingI(SETTINGNAME("aspect"));
		UnderscanSetting = MDFN_GetSettingI(SETTINGNAME("underscan"));
		UndertuneSetting = Area(MDFN_GetSettingI(SETTINGNAME("undertuneleft")), MDFN_GetSettingI(SETTINGNAME("undertunetop")), MDFN_GetSettingI(SETTINGNAME("undertuneright")), MDFN_GetSettingI(SETTINGNAME("undertunebottom")));

		if(aOnLoad || (VsyncSetting != MDFN_GetSettingB(SETTINGNAME("display.vsync"))))
		{
			VsyncSetting = MDFN_GetSettingB(SETTINGNAME("display.vsync"));
			es_video->EnableVsync(VsyncSetting);
		}

		if(aOnLoad || (BorderSetting != MDFN_GetSettingS(SETTINGNAME("shader.border"))))
		{
			BorderSetting = MDFN_GetSettingS(SETTINGNAME("shader.border"));
			if(Utility::FileExists(BorderSetting))
			{
				ImageManager::Purge();
				ImageManager::LoadImage("GameBorderCustom", BorderSetting);
				es_video->AttachBorder(ImageManager::GetImage("GameBorderCustom"));
			}
		}

		if(aOnLoad || (ShaderSetting != MDFN_GetSettingS(SETTINGNAME("shader.preset")) || ShaderPrescaleSetting != MDFN_GetSettingI(SETTINGNAME("shader.prescale"))))
		{
			ShaderSetting = MDFN_GetSettingS(SETTINGNAME("shader.preset"));
			ShaderPrescaleSetting = MDFN_GetSettingI(SETTINGNAME("shader.prescale"));
			es_video->SetFilter(es_paths->Build(std::string("assets/presets/") + ShaderSetting), ShaderPrescaleSetting);
		}
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
			if(strcmp(thisone.name, "shader.preset") == 0)
			{
				thisone.enum_list = BuildShaderEnum();
			}
			//TODO: This strdup will not be freed
			thisone.name = strdup(myname.c_str());
			aSettings.push_back(thisone);
		}
	}
}

bool						MednafenEmu::IsInitialized = false;
bool						MednafenEmu::IsLoaded = false;
bool						MednafenEmu::IsPaused = false;
	
Texture_Ptr					MednafenEmu::Buffer;
MDFN_Surface_Ptr			MednafenEmu::Surface ;
bool						MednafenEmu::SuspendDraw = false;

MDFNGI*						MednafenEmu::GameInfo = 0;
uint8_t*					MednafenEmu::ROMData = 0;
uint32_t					MednafenEmu::ROMSize = 0;
InputHandler_Ptr			MednafenEmu::Inputs;
FastCounter					MednafenEmu::Counter;
EmuRealSyncher				MednafenEmu::Syncher;
LuaScripter*				MednafenEmu::Lua = 0;

std::string					MednafenEmu::Message;
uint32_t					MednafenEmu::MessageTime = 0;
bool						MednafenEmu::RecordingVideo = false;
bool						MednafenEmu::RecordingWave = false;

std::vector<MDFNSetting>	MednafenEmu::Settings;

EmulateSpecStruct			MednafenEmu::EmulatorSpec;	
MDFN_Rect					MednafenEmu::VideoWidths[512];
int16_t						MednafenEmu::Samples[48000];
int16_t						MednafenEmu::SamplesUp[48000];
bool						MednafenEmu::SkipNext = false;
uint32_t					MednafenEmu::SkipCount = 0;
uint32_t					MednafenEmu::FrameCount = 0;

bool						MednafenEmu::RewindSetting = false;
bool						MednafenEmu::DisplayFPSSetting = false;
int32_t						MednafenEmu::AspectSetting = false;
int32_t						MednafenEmu::UnderscanSetting = 10;
std::string					MednafenEmu::ShaderSetting = "";
uint32_t					MednafenEmu::ShaderPrescaleSetting = 1;
Area						MednafenEmu::UndertuneSetting = Area(0, 0, 0, 0);
bool						MednafenEmu::VsyncSetting = true;
std::string					MednafenEmu::BorderSetting = "";

