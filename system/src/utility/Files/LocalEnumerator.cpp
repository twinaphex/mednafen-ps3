#include <ps3_system.h>

void							LocalEnumerator::ListPath				(const std::string& aPath, const std::vector<std::string>& aFilters, std::vector<SummerfaceItem*>& aItems)
{
	std::string Path = Enumerators::CleanPath(aPath);

	std::vector<std::string> items;
	Utility::ListDirectory(Path, items);

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
			SummerfaceItem* item = new SummerfaceItem(items[i], (items[i][items[i].length() - 1] == '/') ? "FolderICON" : "FileICON");
			item->Properties["DIRECTORY"] = (items[i][items[i].length() - 1] == '/') ? "1" : "0";
			item->Properties["FILE"] = (items[i][items[i].length() - 1] == '/') ? "0" : "1";
			item->Properties["PATH"] = aPath + items[i];
			aItems.push_back(item);
		}
	}

}

std::string						LocalEnumerator::ObtainFile				(const std::string& aPath)
{
	return Enumerators::CleanPath(aPath);
}
