#include <mednafen_includes.h>
#include "mednafen_help.h"

#include "SettingCategoryMenu.h"

#include "src/utility/TextViewer.h"
#include "src/utility/Files/FileSelect.h"
#include "src/utility/Files/ArchiveList.h"

namespace
{
	int						FileBrowserHook				(void* aUserData, Summerface* aInterface, const std::string& aWindow, uint32_t aButton)
	{
		if(aButton == ES_BUTTON_AUXRIGHT3)
		{
			SettingCategoryMenu().Do();
			return 1;
		}

		if(aButton == ES_BUTTON_AUXLEFT3)
		{
			Summerface("Text", new TextViewer(Area(10, 10, 80, 80), es_paths->Build("mednafen/Readme.txt").c_str())).Do();
			return 1;
		}

		return 0;
	}

	FileSelect*				FileChooser = 0;
}

void						Exit					()
{
	MednafenEmu::Quit();

	delete FileChooser;

	QuitES();
	exit(0);
}

static std::string			GetFile					()
{
	if(FileChooser == 0)
	{
		FileChooser = new FileSelect("Select ROM", MDFN_GetSettingS("es.bookmarks"), "", new SummerfaceStaticConduit(FileBrowserHook, (void*)0));
	}

	std::string result = FileChooser->GetFile();

	std::string bookmarks;
	MDFNI_SetSetting("es.bookmarks", FileChooser->GetBookmarks(bookmarks).c_str());

	return result;
}

void						ReloadEmulator			(const std::string& aFileName)
{
	std::string filename = aFileName.empty() ? GetFile() : aFileName;

	if(!filename.empty())
	{
		//Load file as an archive
		ArchiveList* archive = new ArchiveList(Area(10, 10, 80, 80), filename);
		Summerface sface("Archive", archive);

		//If there are no items we are lost
		if(archive->IsValid())
		{
			//If there is more than one file, run a list to get the specific file
			if(archive->GetItemCount() > 1)
			{
				sface.Do();

				if(archive->WasCanceled())
				{
					//HACK: If we do this too much we will get a stack overflow, but oh well. Beats a goto anyway
					ReloadEmulator("");
					return;
				}
			}

			//Get the size of the selected file
			uint32_t size = archive->GetSelectedSize();

			//We can only load files up to 64 megabytes, for CD's you should load cue files not BIN files
			if(size)
			{
				void* data = malloc(size);
				if(data)
				{
					if(archive->GetSelectedData(size, data))
					{
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
						if(!MednafenEmu::LoadGame(filename.c_str(), data, size))
						{
							ReloadEmulator("");
							return;
						}
					}
					else
					{
						free(data);

						ESSUB_Error("Could not read file. [Failed to extract]");
						ReloadEmulator("");
						return;
					}
				}
				else
				{
					ESSUB_Error("Could not allocate enough memory to load file. All CD games must be loaded through cue files. [File size too large]");
					ReloadEmulator("");
					return;
				}
			}
			else
			{
				ESSUB_Error("Could not read file. [File empty]");
				ReloadEmulator("");
				return;
			}
		}
		else
		{
			ESSUB_Error("Could not read file. [File not accessible]");
			ReloadEmulator("");
			return;
		}
	}
	else if(!MednafenEmu::IsGameLoaded())
	{
		Exit();
	}
}

int					main					(int argc, char* argv[])
{
	try
	{
		//Init the system and the emulator
		InitES(Exit, argc, argv);

		MednafenEmu::Init();

		//Set the Summerface background
		Summerface::SetDrawBackground(MednafenEmu::DummyFrame);

		//Run the menu
		ReloadEmulator((argc > 1 && argv[1][0] != '-') ? argv[1] : "");

		//Loop
		while(!WantToDie())
		{
			while(WantToSleep() && !WantToDie())
			{
				MednafenEmu::DummyFrame();
				ESVideo::Flip();
			}

			if(MednafenEmu::Frame())
			{
				ESVideo::Flip();
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

