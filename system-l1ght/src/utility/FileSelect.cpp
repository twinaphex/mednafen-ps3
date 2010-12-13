#include <ps3_system.h>

namespace
{
	bool								AlphaSortC								(ListItem* a, ListItem* b)
	{
		if(((FileListItem*)a)->GetDirectory() && !((FileListItem*)b)->GetDirectory())			return true;
		if(((FileListItem*)b)->GetDirectory() && !((FileListItem*)a)->GetDirectory())			return false;
	
		return a->GetText() < b->GetText();
	}
}

										FileListItem::FileListItem				(const std::string& aPath, const std::string& aName, bool aDirectory, bool aBookMark) : ListItem(aName, 0, aDirectory ? "FolderICON" : "FileICON")
{
	FullPath = aPath;
	Directory = aDirectory;
	SetBookMark(aBookMark);
	
	if(ImageManager::GetImage(Utility::GetExtension(aPath) + "ICON"))
	{
		LabelImage = Utility::GetExtension(aPath) + "ICON";
	}
}

std::string								FileListItem::GetPath					()
{
	return FullPath;
}

bool									FileListItem::GetDirectory				()
{
	return Directory;
}

void									FileListItem::SetBookMark				(bool aSet)
{
	BookMark = aSet;
	TextColor = aSet ? Colors::SpecialNormal : Colors::Normal;
	SelectedTextColor = aSet ? Colors::SpecialHighLight : Colors::HighLight;
}

bool									FileListItem::GetBookMark				()
{
	return BookMark;
}


										FileList::FileList						(const std::string& aHeader, const std::string& aPath, std::vector<std::string>& aBookMarks, MenuHook* aInputHook) : WinterfaceList(std::string("[") + aHeader + "]" + aPath, true, true, aInputHook), BookMarks(aBookMarks)
{
	Path = aPath;
	
	CollectFiles(aPath, Items, Utility::StringToVector(aPath == "/" ? "hdd;usb" : "", ';'));

	if(aPath == "/")
	{
		CollectBookMarks(Items);
	}
	
	SideItems.push_back(new ListItem("[DPAD] Navigate", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[X] Select Item", FontManager::GetSmallFont()));	
	SideItems.push_back(new ListItem("[O] Previous Dir", FontManager::GetSmallFont()));	
	SideItems.push_back(new ListItem("[R2] Toggle Bookmark", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[R3] Settings", FontManager::GetSmallFont()));	 //HACK: Only with the mednafen hook!!!!
	SideItems.push_back(new ListItem("[START] Show Readme", FontManager::GetSmallFont()));	 //HACK: Only with the mednafen hook!!!!	

	std::sort(Items.begin(), Items.end(), AlphaSortC);
}

bool									FileList::Input							()
{
	if(PS3Input::ButtonDown(0, PS3_BUTTON_R2))
	{
		FileListItem* item = (FileListItem*)GetSelected();
		std::vector<std::string>::iterator bookmark = std::find(BookMarks.begin(), BookMarks.end(), item->GetPath());
		
		if(bookmark != BookMarks.end())
		{
			BookMarks.erase(bookmark);
			item->SetBookMark(0);
		}
		else
		{
			BookMarks.push_back(item->GetPath());
			item->SetBookMark(1);
		}
	}

	return 	WinterfaceList::Input();
}

std::string								FileList::GetFile						()
{
	return WasCanceled() ? "" : ((FileListItem*)GetSelected())->GetPath();
}

void									FileList::CollectFiles					(const std::string& aPath, std::vector<ListItem*>& aItems, std::vector<std::string> aFilters)
{
	std::vector<std::string> items;
	Utility::ListDirectory(aPath, items);

	for(int i = 0; i != items.size(); i ++)
	{
		bool filterfound = true;
		
		for(int j = 0; j != aFilters.size(); j ++)
		{
			filterfound = false;
			
			if(items[i].find(aFilters[j]) != std::string::npos)
			{
				filterfound = true;
				break;
			}
		}
		
		if(filterfound)
		{
			bool bookmark = std::find(BookMarks.begin(), BookMarks.end(), aPath + items[i]) != BookMarks.end();
			aItems.push_back(new FileListItem(aPath + items[i], items[i], items[i][items[i].length() - 1] == '/', bookmark));
		}
	}
}

void								FileList::CollectBookMarks				(std::vector<ListItem*>& aItems)
{
	for(int i = 0; i != BookMarks.size(); i ++)
	{
		std::string nicename = BookMarks[i];

		if(nicename.empty())
		{
			continue;
		}
		
		if(nicename[nicename.length() - 1] != '/')
		{
			nicename = nicename.substr(nicename.rfind('/') + 1);
		}
		else
		{
			nicename = nicename.substr(0, nicename.length() - 1);
			nicename = nicename.substr(nicename.rfind('/') + 1);
			nicename.push_back('/');
		}
		

		Lv2FsStat statbuf;
		if(0 == lv2FsStat(BookMarks[i].c_str(), &statbuf))
		{
			aItems.push_back(new FileListItem(BookMarks[i], nicename, statbuf.st_mode == 1, true));
		}
	}
}

//TODO: This function is dangerous
										FileSelect::FileSelect				(const std::string& aHeader, std::vector<std::string>& aBookMarks, MenuHook* aInputHook) : BookMarks(aBookMarks)
{
	Header = aHeader;
	InputHook = aInputHook;

	Lists.push(new FileList(Header, "/", BookMarks, InputHook));
}

										FileSelect::~FileSelect				()
{
	while(Lists.size() != 0)
	{
		delete Lists.top();
		Lists.pop();
	}
}


std::string								FileSelect::GetFile					()
{
	std::string result;

	while(!WantToDie())
	{
		Lists.top()->Do();
		
		if(Lists.top()->GetFile().empty())
		{
			if(Lists.size() > 1)
			{
				delete Lists.top();
				Lists.pop();
			}
			else
			{
				break;
			}
			
			continue;
		}
		
		if(Lists.top()->GetFile()[Lists.top()->GetFile().length() - 1] == '/')
		{
			Lists.push(new FileList(Header, Lists.top()->GetFile(), BookMarks, InputHook));
			continue;
		}

		result = Lists.top()->GetFile();
		break;
	}

	return result;
}
