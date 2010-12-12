#include <mednafen_includes.h>

extern "C" int gettimeofday(){}
extern "C" int getrusage(){}
extern "C" int dup(int){}

PathBuild			Paths("/dev_hdd0/game/MDFN90002/USRDIR");

namespace
{
	class			MDFNInputHook : public MenuHook
	{
		public:
			bool	Input	()
			{
				if(PS3Input::ButtonDown(0, PS3_BUTTON_R3))
				{
					MednafenSettings("general").Do();
				}
				
				if(PS3Input::ButtonDown(0, PS3_BUTTON_START))
				{
					TextViewer(Paths.Build("mednafen/Readme.txt")).Do();
				}
				
				return false;
			}
	};
	
	MDFNInputHook	InputHook;
	FileSelect*		Browser = 0;
	std::vector<std::string>	bookmarks;
};


void				Exit					()
{
	MednafenEmu::Quit();
	
	QuitPS3();
	exit(0);
}


void				ReloadEmulator			()
{
	bookmarks = Utility::StringToVector(MDFN_GetSettingS("ps3.bookmarks"), ';');
	
	if(!Browser)
	{
		Browser = new FileSelect("Select ROM", bookmarks, &InputHook);
	}
	
	std::string romfilename = Browser->GetFile();
	MDFNI_SetSetting("ps3.bookmarks", Utility::VectorToString(bookmarks, ';').c_str());

	if(romfilename.length() == 0 && !MednafenEmu::IsGameLoaded())
	{
		Exit();
	}
	else if(romfilename.length() == 0 && MednafenEmu::IsGameLoaded())
	{
		return;
	}
	else
	{
		ArchiveList archive(std::string("[Select ROM] ") + romfilename, romfilename);
		
		if(archive.ItemCount() == 0)
		{
			Exit();
		}
		
		if(archive.ItemCount() > 1)
		{
			archive.Do();
			
			if(archive.WasCanceled())
			{
				ReloadEmulator();
				return;
			}
		}
		
		uint32_t size;
		void* data;

		size = archive.GetSelectedSize();
		data = malloc(size);
		archive.GetSelectedData(size, data);
		
		if(ArchiveList::IsArchive(romfilename) && romfilename.rfind('/') != std::string::npos)
		{
			romfilename = romfilename.substr(0, romfilename.rfind('/') + 1);
			romfilename += archive.GetSelectedFileName();
		}
		else
		{
			romfilename = archive.GetSelectedFileName();
		}
		
	
		MednafenEmu::CloseGame();
		MednafenEmu::LoadGame(romfilename, data, size);
		
		if(data)
		{
			free(data);
		}
	}
}

int					main					()
{
	try
	{
		InitPS3(Exit);
	
		MednafenEmu::Init();

		ReloadEmulator();

		while(!WantToDie())
		{
			while(WantToSleep() && !WantToDie())
			{
				MednafenEmu::DummyFrame();
			}
	
			PS3Audio::Block();
	
			MednafenEmu::Frame();
		}
	
		Exit();
	}
	catch(const char* s)
	{
		printf("EXCEPTION: %s\n\n", s);
		Exit();
	}
}
