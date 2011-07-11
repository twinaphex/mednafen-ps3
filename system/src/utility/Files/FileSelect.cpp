#include <es_system.h>

namespace
{
	bool								AlphaSortDirectory					(SummerfaceItem_Ptr a, SummerfaceItem_Ptr b)
	{
		if(a->IntProperties["DIRECTORY"] == 1 && b->IntProperties["DIRECTORY"] == 0)		return true;
		if(a->IntProperties["DIRECTORY"] == 0 && b->IntProperties["DIRECTORY"] == 1)		return false;
		return a->GetText() < b->GetText();
	}
}


										FileSelect::FileSelect				(const std::string& aHeader, BookmarkList& aBookMarks, const std::string& aPath, SummerfaceInputConduit_Ptr aInputHook) :
	List(smartptr::make_shared<SummerfaceList>(Area(10, 10, 80, 80))),
	Interface(Summerface::Create("List", List)),
	Header(aHeader),
	BookMarks(aBookMarks)
{
	if(aInputHook)
	{
		Interface->AttachConduit(aInputHook);
	}

	Interface->AttachConduit(smartptr::make_shared<SummerfaceTemplateConduit<FileSelect> >(this));

	Paths.push(aPath);
	LoadList(aPath);
}

int										FileSelect::HandleInput				(Summerface_Ptr aInterface, const std::string& aWindow)
{
	//Use the input conduit to toggle bookmarks
	if(ESInput::ButtonDown(0, ES_BUTTON_AUXRIGHT2))
	{
		SummerfaceItem_Ptr item = List->GetSelected();
		BookmarkList::iterator bookmark = std::find(BookMarks.begin(), BookMarks.end(), item->Properties["PATH"]);
		
		if(bookmark != BookMarks.end())
		{
			BookMarks.erase(bookmark);
			item->IntProperties["BOOKMARK"] = 0;
			item->SetColors(Colors::Normal, Colors::HighLight);
		}
		else
		{
			BookMarks.push_back(item->Properties["PATH"]);
			item->IntProperties["BOOKMARK"] = 1;
			item->SetColors(Colors::SpecialNormal, Colors::SpecialHighLight);
		}

		//Eat the input
		return 1;
	}

	return 0;
}

std::string								FileSelect::GetFile					()
{
	std::string result;

	while(!WantToDie())
	{
		//Run the list
		Interface->Do();

		//If the list was canceled; try to move up the stack
		if(List->WasCanceled())
		{
			//Go to parent directory
			if(Paths.size() > 1)
			{
				Paths.pop();
				LoadList(Paths.top().c_str());
				continue;
			}
			//No parent, return empty string
			else
			{
				return "";
			}
		}
		
		//If a directory was selected, list it
		if(List->GetSelected()->IntProperties["DIRECTORY"])
		{
			Paths.push(List->GetSelected()->Properties["PATH"]);
			LoadList(Paths.top());
		}
		//If a file was selected, return it
		else
		{
			return List->GetSelected()->Properties["PATH"];
			break;
		}
	}

	return "";
}

void								FileSelect::LoadList						(const std::string& aPath)
{
	//Prep the list for this directory
	List->ClearItems();
	List->SetHeader("[%s] %s", Header.c_str(), aPath.c_str());
	List->SetView(smartptr::make_shared<AnchoredListView>(List, true));

	//If the path is empty, list the drive selection and bookmarks
	//TODO: Support drive selection on windows
	if(aPath.empty())
	{
		List->AddItem(MakeItem("Local Files", "/", true, false));

		//Load bookmarks
		for(BookmarkList::iterator i = BookMarks.begin(); i != BookMarks.end(); i ++)
		{
			if(!i->empty())
			{
				std::string nicename = *i;

				bool directory = false;
				if(nicename[nicename.length() - 1] != '/')
				{
						nicename = nicename.substr(nicename.rfind('/') + 1);
				}
				else
				{
						nicename = nicename.substr(0, nicename.length() - 1);
						nicename = nicename.substr(nicename.rfind('/') + 1);
						nicename.push_back('/');
						directory = true;
				}
				
				List->AddItem(MakeItem(nicename, *i, directory, !directory));
			}
		}
	}
	else
	{
		std::list<std::string> items;
		Utility::ListDirectory(aPath, items);

		for(std::list<std::string>::iterator i = items.begin(); i != items.end(); i ++)
		{
			List->AddItem(MakeItem(*i, aPath + *i, (*i)[i->length() - 1] == '/', (*i)[i->length() - 1] != '/'));
		}

		List->Sort(AlphaSortDirectory);
	}
}

SummerfaceItem_Ptr					FileSelect::MakeItem						(const std::string& aName, const std::string& aPath, bool aDirectory, bool aFile)
{
	std::string extension = Utility::GetExtension(aPath);

	SummerfaceItem_Ptr item = smartptr::make_shared<SummerfaceItem>(aName, aDirectory ? "FolderICON" : (ImageManager::GetImage(extension + "ICON") ? extension + "ICON" : "FileICON"));
	item->IntProperties["DIRECTORY"] = aDirectory;
	item->IntProperties["FILE"] = aFile;
	item->IntProperties["BOOKMARK"] = std::find(BookMarks.begin(), BookMarks.end(), aPath) != BookMarks.end();
	item->Properties["PATH"] = aPath;
	item->Properties["THUMB"] = Utility::GetDirectory(aPath) + "/__images/" + Utility::GetFileName(aPath) + ".png";

	if(item->IntProperties["BOOKMARK"])
	{
		item->SetColors(Colors::SpecialNormal, Colors::SpecialHighLight);
	}

	return item;
}

