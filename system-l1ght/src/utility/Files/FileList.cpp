#include <ps3_system.h>

namespace
{
	bool								AlphaSortC								(ListItem* a, ListItem* b)
	{
		if(((FileListItem*)a)->IsDirectory() && !((FileListItem*)b)->IsDirectory())			return true;
		if(((FileListItem*)b)->IsDirectory() && !((FileListItem*)a)->IsDirectory())			return false;
	
		return a->GetText() < b->GetText();
	}
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
			aItems.push_back(new FileListItem(items[i], aPath + items[i], items[i][items[i].length() - 1] == '/', bookmark));
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
			aItems.push_back(new FileListItem(nicename, BookMarks[i], statbuf.st_mode == 1, true));
		}
	}
}
