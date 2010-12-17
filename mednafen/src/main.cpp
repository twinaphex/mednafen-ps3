#include <mednafen_includes.h>

extern "C" int gettimeofday(timeval*, void*){}
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
	
	MDFNInputHook				InputHook;
	Browser* 					FileChooser;
	std::vector<std::string>	bookmarks;
};


void				Exit					()
{
	MednafenEmu::Quit();
	
	QuitPS3();
	exit(0);
}

std::string			GetFile					()
{
	bookmarks = Utility::StringToVector(MDFN_GetSettingS("ps3.bookmarks"), ';');

	if(FileChooser == 0)
	{
		FileChooser = new Browser("Select ROM", MDFN_GetSettingS("ftp.ps3.host"), MDFN_GetSettingS("ftp.ps3.port"), "", "", bookmarks, &InputHook);
	}
	
	FileChooser->Do();
	MDFNI_SetSetting("ps3.bookmarks", Utility::VectorToString(bookmarks, ';').c_str());
	
	return FileChooser->SelectedFile();

/*	if(MDFN_GetSettingB("ftp.ps3.enable"))
	{
		FTPBrowser = new FTPSelect("Select ROM", , &InputHook);
		std::string file = FTPBrowser->GetFile();
		
		if(file.empty())
		{
			return "";
		}
		
		FTPBrowser->DownloadFile("/dev_hdd0/game/MDFN90002/USRDIR/");
		
		return std::string("/dev_hdd0/game/MDFN90002/USRDIR/") + FTPBrowser->GetFileName();
	}
	else
	{

		if(!Browser)
		{
			Browser = new FileSelect("Select ROM", bookmarks, &InputHook);
		}
		
		romfilename = Browser->GetFile();
		MDFNI_SetSetting("ps3.bookmarks", Utility::VectorToString(bookmarks, ';').c_str());
		
		if(!romfilename.empty())
		{
			return romfilename.c_str();
		}
		else
		{
			return 0;
		}
	}*/
}

void				ReloadEmulator			()
{
	std::string filename = GetFile();	

	if(filename.empty() && !MednafenEmu::IsGameLoaded())
	{
		Exit();
	}
	else if(filename.empty() && MednafenEmu::IsGameLoaded())
	{
		return;
	}
	else
	{
		ArchiveList archive(std::string("[Select ROM] ") + filename, filename);
		
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
		
		if(ArchiveList::IsArchive(filename) && filename.rfind('/') != std::string::npos)
		{
			filename = filename.substr(0, filename.rfind('/') + 1);
			filename += archive.GetSelectedFileName();
		}
		else
		{
			filename = archive.GetSelectedFileName();
		}
		
	
		MednafenEmu::CloseGame();
		MednafenEmu::LoadGame(filename, data, size);
		
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
