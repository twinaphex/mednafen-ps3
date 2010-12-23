#include <ps3_system.h>

void							LocalEnumerator::ListPath				(const std::string& aPath, const std::vector<std::string>& aFilters, std::vector<ListItem*>& aItems)
{
	std::string Path = Enumerators::CleanPath(aPath);

	std::vector<std::string> items;
	Utility::ListDirectory(Path, items);

	for(int i = 0; i != items.size(); i ++)
	{
		bool filterfound = true;

		if(Path == "/")
		{
			if(items[i].find("Users") == std::string::npos)
			{
				continue;
			}
		}
		
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
			aItems.push_back(new FileListItem(items[i], aPath + items[i], items[i][items[i].length() - 1] == '/', false));
		}
	}

}

std::string						LocalEnumerator::ObtainFile				(const std::string& aPath)
{
	return Enumerators::CleanPath(aPath);
}
