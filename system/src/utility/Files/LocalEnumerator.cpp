#include <es_system.h>

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
			aItems.push_back(FileList::MakeItem(items[i], aPath + items[i], items[i][items[i].length() - 1] == '/', items[i][items[i].length() - 1] != '/', false));
		}
	}

}

std::string						LocalEnumerator::ObtainFile				(const std::string& aPath)
{
	return Enumerators::CleanPath(aPath);
}
