#include <mednafen_includes.h>
#include "mednafen_help.h"
#include "settingmenu.h"

namespace
{
	int				FileBrowserHook				(void* aUserData, Summerface_Ptr aInterface, const std::string& aWindow, uint32_t aButton)
	{
		if(aButton == ES_BUTTON_AUXRIGHT3)
		{
			SettingMenu().Do();
			return 1;
		}

		if(aButton == ES_BUTTON_AUXLEFT3)
		{
			Summerface::Create("Text", smartptr::make_shared<TextViewer>(Area(10, 10, 80, 80), es_paths->Build("mednafen/Readme.txt")))->Do();
			return 1;
		}

		return 0;
	}

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
		FileChooser = new FileSelect("Select ROM", bookmarks, "", smartptr::make_shared<SummerfaceStaticConduit>(FileBrowserHook, (void*)0));
	}

	std::string result = FileChooser->GetFile();
	MDFNI_SetSetting("es.bookmarks", Utility::VectorToString(bookmarks, ';').c_str());

	return result;
}

void				ReloadEmulator			(const std::string& aFileName)
{
	std::string filename = aFileName.empty() ? GetFile() : aFileName;

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
		//Load file as an archive
//		ArchiveList_Ptr archive = smartptr::make_shared<ArchiveList>(Area(10, 10, 80, 80), filename);
		smartptr::shared_ptr<ArchiveList> archive = smartptr::make_shared<ArchiveList>(Area(10, 10, 80, 80), filename);
		archive->SetView(smartptr::make_shared<AnchoredListView>(archive, true));
		Summerface_Ptr sface = Summerface::Create("Archive", archive);

		//If there are no items we are lost
		ErrorCheck(archive->GetItemCount() != 0, "Loader: Could not read file [File: %s]", filename.c_str());

		//If there is more than one file, run a list to get the specific file
		if(archive->GetItemCount() > 1)
		{
			sface->Do();

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
		if(size < 64 * 1024 * 1024)
		{
			void* data = malloc(size);
			ErrorCheck(data, "Loader: Failed to allocate memory for file [File: %s, Size: %d]", filename.c_str(), size);

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
		}
		else
		{
			ESSUB_Error("File is larger than 64MB. Not supported, if this is a CD game you must load it through a cue file.");
			ReloadEmulator("");
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

		//Set the Summerface background
		Summerface::SetDrawBackground(MednafenEmu::DummyFrame);

		//Run the menu
		ReloadEmulator((argc > 1) ? argv[1] : "");

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
#ifdef USE_ERRORCHECK
	catch(ESException& s)
	{
		printf("EXCEPTION: %s\n\n", s.what());
		Exit();
		abort();
	}
#endif
	catch(std::exception& s)
	{
		printf("EXCEPTION: %s\n\n", s.what());
		Exit();
		abort();
	}
}

