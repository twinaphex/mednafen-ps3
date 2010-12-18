#include <ps3_system.h>

//HACK: Can't put them in FileEnumerator.h, so put them here
EnumeratorEnumerator			Enumerators::Root;
LocalEnumerator					Enumerators::Local;
FTPEnumerator					Enumerators::FTP;

void							EnumeratorEnumerator::ListPath				(const std::string& aPath, const std::vector<std::string>& aFilters, std::vector<ListItem*>& aItems)
{
	aItems.push_back(new FileListItem("Local Storage", "file:/", true, false));
	aItems.push_back(new FileListItem("FTP Storage", "ftp:/", true, false));	
}

std::string						EnumeratorEnumerator::ObtainFile			(const std::string& aPath)
{
	return "";
}
