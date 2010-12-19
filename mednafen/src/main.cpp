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
	FileSelect*					FileChooser = 0;
	std::vector<std::string>	bookmarks;
};


void				Exit					()
{
	MednafenEmu::Quit();
	
	delete FileChooser;
	
	QuitPS3();
	exit(0);
}

std::string			GetFile					()
{
	bookmarks = Utility::StringToVector(MDFN_GetSettingS("ps3.bookmarks"), ';');

	if(FileChooser == 0)
	{
		FileChooser = new FileSelect("Select ROM", bookmarks, "", &InputHook);
	}
	
	std::string result = FileChooser->GetFile();
	MDFNI_SetSetting("ps3.bookmarks", Utility::VectorToString(bookmarks, ';').c_str());
	
	return result;
}

void				ReloadEmulator			()
{
	std::string enumpath = GetFile();

	if(enumpath.empty() && !MednafenEmu::IsGameLoaded())
	{
		Exit();
	}
	else if(enumpath.empty() && MednafenEmu::IsGameLoaded())
	{
		return;
	}
	else
	{
		std::string filename = Enumerators::GetEnumerator(enumpath).ObtainFile(enumpath);
	
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
		
	
		if(filename.rfind("/") != std::string::npos)
		{
			filename = filename.substr(filename.rfind("/") + 1);
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

		FTPEnumerator::SetCredentials(MDFN_GetSettingS("ftp.ps3.host"), MDFN_GetSettingS("ftp.ps3.port"), MDFN_GetSettingS("ftp.ps3.username"), MDFN_GetSettingS("ftp.ps3.password"));

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
