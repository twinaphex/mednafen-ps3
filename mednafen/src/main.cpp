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
					MednafenSettings::Do();
				}

				if(es_input->ButtonDown(0, ES_BUTTON_AUXLEFT3))
				{
					Summerface("Text", new TextViewer(Area(10, 10, 80, 80), es_paths->Build("mednafen/Readme.txt"))).Do();
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
		ArchiveList* archive = new ArchiveList(Area(10, 10, 80, 80), filename);
		Summerface sface("Archive", archive);

		//If there are no items we are lost
		if(archive->GetItemCount() == 0)
		{
			throw ESException("Loader: Could not read file [File: %s]", enumpath.c_str());
		}

		//If there is more than one file, run a list to get the specific file
		if(archive->GetItemCount() > 1)
		{
			sface.Do();

			if(archive->WasCanceled())
			{
				//HACK: If we do this too much we will get a stack overflow, but oh well. Beats a goto anyway
				ReloadEmulator();
				return;
			}
		}

		//Get the size of the selected file
		uint32_t size = archive->GetSelectedSize();

		//We can only load files up to 64 megabytes, for CD's you should load cue files not BIN files
		if(size < 64 * 1024 * 1024)
		{
			void* data = malloc(size);
			if(!data)
			{
				throw ESException("Loader: Failed to allocate memory for file [File: %s, Size: %d]", filename.c_str(), size);
			}

			archive->GetSelectedData(size, data);

			//Clean up the filename, if we are loading a file from an archive replace the name of the zip file with the name of the file inside
			if(ArchiveList::IsArchive(filename) && filename.rfind('/') != std::string::npos)
			{
				filename = filename.substr(0, filename.rfind('/') + 1);
				filename += archive->GetSelectedFileName();
			}
			else
			{
				filename = archive->GetSelectedFileName();
			}

			//Load the game into mednafen
			MednafenEmu::CloseGame();
			MednafenEmu::LoadGame(filename, data, size);

			//Clean up and leave
			free(data);
		}
		else
		{
			ESSUB_Error("File is larger than 64MB. Not supported, if this is a CD game you must load it through a cue file.");
			ReloadEmulator();
		}
	}
}

static void			SummerfaceBackground	()
{
	if(MednafenEmu::DummyFrame())
	{
		es_video->FillRectangle(Area(0, 0, es_video->GetScreenWidth(), es_video->GetScreenHeight()), 0x00000080);
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

		//Set the Summerface background
		Summerface::SetDrawBackground(SummerfaceBackground);

		//Run the menu
		ReloadEmulator();

		//Loop
		while(!WantToDie())
		{
			while(WantToSleep() && !WantToDie())
			{
				MednafenEmu::DummyFrame();
				es_video->Flip();
			}

			if(MednafenEmu::Frame())
			{
				es_video->Flip();
			}
		}
	
		Exit();
	}
	catch(const char* s)
	{
		printf("%s\n", s);
		Exit();
		abort();
	}
	catch(ESException& s)
	{
		printf("EXCEPTION: %s\n\n", s.what());
		Exit();
		abort();
	}
	catch(std::exception& s)
	{
		printf("EXCEPTION: %s\n\n", s.what());
		Exit();
		abort();
	}
}
