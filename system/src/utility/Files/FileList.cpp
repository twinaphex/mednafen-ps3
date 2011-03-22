#include <es_system.h>

namespace
{
	bool								AlphaSortC								(SummerfaceItem* a, SummerfaceItem* b)
	{
		if(a->IntProperties["DIRECTORY"] && !b->IntProperties["DIRECTORY"])			return true;
		if(b->IntProperties["DIRECTORY"] && !a->IntProperties["DIRECTORY"])			return false;
	
		return a->GetText() < b->GetText();
	}
}

										FileList::FileList						(const Area& aRegion, const std::string& aPath, std::vector<std::string>& aBookmarks) : SummerfaceList(aRegion), BookMarks(aBookmarks)
{
	FileEnumerator& enumer = Enumerators::GetEnumerator(aPath);
//	SetModel(new GridListModel(this, 4, 4, false, true));

	Path = aPath;

	if(aPath.empty())
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
			
			AddItem(MakeItem(nicename, BookMarks[i], directory, !directory, true));
		}
	}

	std::vector<std::string> filters;

	std::vector<SummerfaceItem*> items;
	enumer.ListPath(aPath, filters, items);

	for(int i = 0; i != items.size(); i ++)
	{
		if(Utility::GetExtension(items[i]->Properties["PATH"]) == "tbn")
		{
			delete items[i];
		}
		else
		{	
			AddItem(items[i]);
		}
	}

	Sort(AlphaSortC);
}

bool									FileList::Input							()
{
	if(es_input->ButtonDown(0, ES_BUTTON_AUXRIGHT2))
	{
		SummerfaceItem* item = GetSelected();
		std::vector<std::string>::iterator bookmark = std::find(BookMarks.begin(), BookMarks.end(), item->Properties["PATH"]);
		
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

		return false;
	}

	return 	SummerfaceList::Input();
}

std::string								FileList::GetFile						()
{
	if(WasCanceled())
	{
		throw ESException("FileList::GetFile: Can't get a file if the list was canceled");
	}

	return GetSelected()->Properties["PATH"];
}

bool									FileList::IsDirectory					()
{
	if(WasCanceled())
	{
		throw ESException("FileList::IsDirectory: Can't get a file if the list was canceled");
	}
	
	return GetSelected()->IntProperties["DIRECTORY"];
}

SummerfaceItem*							FileList::MakeItem						(const std::string& aName, const std::string& aPath, bool aDirectory, bool aFile, bool aBookMark)
{
	std::string extension = Utility::GetExtension(aPath);

	SummerfaceItem* item = new SummerfaceItem(aName, aDirectory ? "FolderICON" : (ImageManager::GetImage(extension + "ICON") ? extension + "ICON" : "FileICON"));
	item->IntProperties["DIRECTORY"] = aDirectory;
	item->IntProperties["FILE"] = aFile;
	item->IntProperties["BOOKMARK"] = aBookMark;
	item->Properties["PATH"] = aPath;
	item->Properties["THUMB"] = Enumerators::CleanPath(aPath) + ".tbn";

	if(aBookMark)
	{
		item->SetColors(Colors::SpecialNormal, Colors::SpecialHighLight);
	}

	return item;
}

