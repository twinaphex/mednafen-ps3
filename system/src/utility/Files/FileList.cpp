#include <ps3_system.h>

namespace
{
	bool								AlphaSortC								(SummerfaceItem* a, SummerfaceItem* b)
	{
		if(a->Properties["DIRECTORY"] == "1" && b->Properties["DIRECTORY"] == "0")			return true;
		if(b->Properties["DIRECTORY"] == "1" && a->Properties["DIRECTORY"] == "0")			return false;
	
		return a->GetText() < b->GetText();
	}
}

										FileList::FileList						(const Area& aRegion, const std::string& aHeader, const std::string& aPath, std::vector<std::string>& aBookmarks) : SummerfaceLineList(aRegion), BookMarks(aBookmarks)
{
	FileEnumerator& enumer = Enumerators::GetEnumerator(aPath);

	Path = aPath;

/*	if(aPath.empty())
	{
		for(int i = 0; i != BookMarks.size(); i ++)
		{
			std::string nicename = BookMarks[i];
	
			//TODO: Validate path
	
			if(nicename.empty())
			{
				continue;
			}
			
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
			

			Items.push_back(new FileListItem(nicename, BookMarks[i], directory, true));
		}
	}*/

	std::vector<std::string> filters;
	std::vector<SummerfaceItem*> items;
	enumer.ListPath(aPath, filters, items);
	for(int i = 0; i != items.size(); i ++)
	{
		AddItem(items[i]);
	}
	
//	std::sort(Items.begin(), Items.end(), AlphaSortC);
}

bool									FileList::Input							()
{
	if(es_input->ButtonDown(0, ES_BUTTON_AUXRIGHT2))
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

	return 	SummerfaceLineList::Input();
}

std::string								FileList::GetFile						()
{
	if(WasCanceled())
	{
		throw FileException("FileList::GetFile: Can't get a file if the list was canceled");
	}

	return ((FileListItem*)GetSelected())->GetPath();
}

bool									FileList::IsDirectory					()
{
	if(WasCanceled())
	{
		throw FileException("FileList::IsDirectory: Can't get a file if the list was canceled");
	}
	
	return ((FileListItem*)GetSelected())->IsDirectory();
}
