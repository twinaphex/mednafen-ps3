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


										FileSelect::FileSelect				(const std::string& aHeader, BookmarkList& aBookMarks, const std::string& aPath, MenuHook* aInputHook) :
	List(boost::make_shared<SummerfaceList>(Area(10, 10, 80, 80))),
	Interface(Summerface::Create("List", List)),
	Valid(true),
	Header(aHeader),
	BookMarks(aBookMarks)
{
	Interface->SetHook(aInputHook);
	List->SetInputConduit(boost::make_shared<SummerfaceTemplateConduit<FileSelect> >(this));

	Paths.push(aPath);
	LoadList(aPath);
}

bool									FileSelect::HandleInput				(Summerface_Ptr aInterface, const std::string& aWindow)
{
	if(es_input->ButtonDown(0, ES_BUTTON_AUXRIGHT2))
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
	}

	return false;
}

std::string								FileSelect::GetFile					()
{
	if(Valid)
	{
		std::string result;
	
		while(!WantToDie())
		{
			Interface->Do();

			if(List->WasCanceled())
			{
				if(Paths.size() > 1)
				{
					Paths.pop();
					LoadList(Paths.top().c_str());
					continue;
				}
				else
				{
					return "";
				}
			}
			
			if(List->GetSelected()->IntProperties["DIRECTORY"])
			{
				Paths.push(List->GetSelected()->Properties["PATH"]);
				LoadList(Paths.top());
			}
			else
			{
				return List->GetSelected()->Properties["PATH"];
				break;
			}
		}
	
		return "";
	}
	else
	{
		throw ESException("FileSelect::GetFile: FileSelect object is invalid.");
	}
}

void								FileSelect::LoadList						(const std::string& aPath)
{
	List->ClearItems();
	List->SetHeader("[%s] %s", Header.c_str(), aPath.c_str());

	if(Utility::DirectoryExists(aPath + "/__images"))
	{
		List->SetView(boost::make_shared<GridListView>(List, 4, 3, true, false));
	}
	else
	{
		List->SetView(boost::make_shared<AnchoredListView>(List, true));
	}

	if(aPath.empty())
	{
		List->AddItem(MakeItem("Local Files", "/", true, false));

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
		std::vector<std::string> items;
		Utility::ListDirectory(aPath, items);

		for(int i = 0; i != items.size(); i ++)
		{
			List->AddItem(MakeItem(items[i], aPath + items[i], items[i][items[i].length() - 1] == '/', items[i][items[i].length() - 1] != '/'));
		}

		List->Sort(AlphaSortDirectory);
	}
}

SummerfaceItem_Ptr					FileSelect::MakeItem						(const std::string& aName, const std::string& aPath, bool aDirectory, bool aFile)
{
	std::string extension = Utility::GetExtension(aPath);

	SummerfaceItem_Ptr item = boost::make_shared<SummerfaceItem>(aName, aDirectory ? "FolderICON" : (ImageManager::GetImage(extension + "ICON") ? extension + "ICON" : "FileICON"));
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

