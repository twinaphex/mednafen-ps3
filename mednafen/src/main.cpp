#include <mednafen_includes.h>

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
					TextViewer(es_paths->Build("mednafen/Readme.txt")).Do();
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
		//Get the real filename of the local file (already downloaded to a normal location) and open it in fex
		std::string filename = Enumerators::GetEnumerator(enumpath).ObtainFile(enumpath);
		ArchiveList archive(std::string("[Select ROM] ") + enumpath, filename);

		//If there are no items we are lost
		if(archive.ItemCount() == 0)
		{
			throw ESException("Loader: Could not read file [File: %s]", enumpath.c_str());
		}

		//If there is more than one file, run a list to get the specific file
		if(archive.ItemCount() > 1)
		{
			archive.Do();

			if(archive.WasCanceled())
			{
				//HACK: If we do this too much we will get a stack overflow, but oh well. Beats a goto anyway
				ReloadEmulator();
				return;
			}
		}

		//Get the size of the selected file
		uint32_t size;
		void* data = 0;

		size = archive.GetSelectedSize();

		//We can only load files up to 64 megabytes, for CD's you should load cue files not BIN files
		if(size < 64 * 1024 * 1024)
		{
			data = malloc(size);
			archive.GetSelectedData(size, data);

			//Clean up the filename, if we are loading a file from an archive replace the name of the zip file with the name of the file inside
			if(ArchiveList::IsArchive(filename) && filename.rfind('/') != std::string::npos)
			{
				filename = filename.substr(0, filename.rfind('/') + 1);
				filename += archive.GetSelectedFileName();
			}
			else
			{
				filename = archive.GetSelectedFileName();
			}
		}
		else
		{
			throw ESException("Loader: File is larger than 64MB, can't open [File: %s, Size: %d]", enumpath.c_str(), size);
		}

		//Load the game into mednafen
		MednafenEmu::CloseGame();
		MednafenEmu::LoadGame(filename, data, size);

		//Clean up and leave
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
		//Init the system and the emulator
		InitES(Exit);
		MednafenEmu::Init();

		//Set FTP settings
		FTPEnumerator::SetEnabled(MDFN_GetSettingB("ftp.es.enable"));
		FTPEnumerator::SetCredentials(MDFN_GetSettingS("ftp.es.host"), MDFN_GetSettingS("ftp.es.port"), MDFN_GetSettingS("ftp.es.username"), MDFN_GetSettingS("ftp.es.password"));

		//Run the menu
		ReloadEmulator();

		//Loop
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
		printf("%s\n", s);
		Exit();
	}
	catch(ESException& s)
	{
		printf("EXCEPTION: %s\n\n", s.what());
		Exit();
	}
	catch(std::exception& s)
	{
		printf("EXCEPTION: %s\n\n", s.what());
		Exit();
	}
}
