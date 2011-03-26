#include <es_system.h>

										FileSelect::FileSelect				(const std::string& aHeader, std::vector<std::string>& aBookMarks, const std::string& aPath, MenuHook* aInputHook) :
	List(Area(10, 10, 80, 80)),
	Interface("List", &List),
	Valid(true),
	Header(aHeader),
	BookMarks(aBookMarks)

{
	List.SetNoDelete();
	List.SetHeader("[%s] %s", Header.c_str(), aPath.c_str());

	Interface.SetHook(aInputHook);

	Paths.push(aPath);
	LoadList(aPath.empty() ? "/" : aPath.c_str());
}

std::string								FileSelect::GetFile					()
{
	if(Valid)
	{
		std::string result;
	
		while(!WantToDie())
		{
			Interface.Do();

			if(List.WasCanceled())
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
			
			if(List.GetSelected()->IntProperties["DIRECTORY"])
			{
				Paths.push(List.GetSelected()->Properties["PATH"]);
				List.SetHeader("[%s] %s", Header.c_str(), List.GetSelected()->Properties["PATH"].c_str());

				LoadList(Paths.top().c_str());
			}
			else
			{
				return List.GetSelected()->Properties["PATH"];
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

void								FileSelect::LoadList						(const char* aPath)
{
	assert(aPath);

	List.ClearItems();

	std::vector<std::string> items;
	Utility::ListDirectory(aPath, items);

	for(int i = 0; i != items.size(); i ++)
	{
		List.AddItem(MakeItem(items[i], aPath + items[i], items[i][items[i].length() - 1] == '/', items[i][items[i].length() - 1] != '/', false));
	}
}

SummerfaceItem*						FileSelect::MakeItem						(const std::string& aName, const std::string& aPath, bool aDirectory, bool aFile, bool aBookMark)
{
	std::string extension = Utility::GetExtension(aPath);

	SummerfaceItem* item = new SummerfaceItem(aName, aDirectory ? "FolderICON" : (ImageManager::GetImage(extension + "ICON") ? extension + "ICON" : "FileICON"));
	item->IntProperties["DIRECTORY"] = aDirectory;
	item->IntProperties["FILE"] = aFile;
	item->IntProperties["BOOKMARK"] = aBookMark;
	item->Properties["PATH"] = aPath;
	item->Properties["THUMB"] = aPath + ".tbn";

	if(aBookMark)
	{
		item->SetColors(Colors::SpecialNormal, Colors::SpecialHighLight);
	}

	return item;
}

