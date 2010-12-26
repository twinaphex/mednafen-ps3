#include <mednafen_includes.h>

///HACK: These might not be nessicary anymore, include them anyway
#ifdef L1GHT
extern "C" int gettimeofday(timeval*, void*){}
extern "C" int getrusage(){}
extern "C" int dup(int){}

PathBuild			Paths("/dev_hdd0/game/MDFN90002/USRDIR/");
#else
//HACK: Hardcoded path, what a joke
PathBuild			Paths("/Users/jason/.mednafen/");
#endif

namespace
{
	class			MDFNInputHook : public MenuHook
	{
		public:
			bool	Input	()
			{
				if(es_input->ButtonDown(0, ES_BUTTON_AUXRIGHT3))
				{
					MednafenSettings("general").Do();
				}
				
				if(es_input->ButtonDown(0, ES_BUTTON_AUXLEFT3))
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
	
	QuitES();
	exit(0);
}

std::string			GetFile					()
{
	bookmarks = Utility::StringToVector(MDFN_GetSettingS("es.bookmarks"), ';');

	if(FileChooser == 0)
	{
		FileChooser = new FileSelect("Select ROM", bookmarks, "", &InputHook);
	}
	
	std::string result = FileChooser->GetFile();
	MDFNI_SetSetting("es.bookmarks", Utility::VectorToString(bookmarks, ';').c_str());
	
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
	
		ArchiveList archive(std::string("[Select ROM] ") + enumpath, filename);
		
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

int					main					(int argc, char* argv[])
{
	try
	{
		InitES(Exit);
	
		MednafenEmu::Init();

		FTPEnumerator::SetEnabled(MDFN_GetSettingB("ftp.es.enable"));
		FTPEnumerator::SetCredentials(MDFN_GetSettingS("ftp.es.host"), MDFN_GetSettingS("ftp.es.port"), MDFN_GetSettingS("ftp.es.username"), MDFN_GetSettingS("ftp.es.password"));

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
	catch(char* s)
	{
		printf("%s\n", s);
		Exit();
	}
	catch(std::exception s)
	{
		printf("EXCEPTION: %s\n\n", s.what());
		Exit();
	}
}
