#include <ps3_system.h>

//HACK: Can't put them in FileEnumerator.h, so put them here
EnumeratorEnumerator			Enumerators::Root;
LocalEnumerator					Enumerators::Local;
FTPEnumerator					Enumerators::FTP;

void							EnumeratorEnumerator::ListPath				(const std::string& aPath, const std::vector<std::string>& aFilters, std::vector<SummerfaceItem*>& aItems)
{
	SummerfaceItem* item = new SummerfaceItem("Local Storage", "FolderICON");
	item->Properties["DIRECTORY"] = "1";
	item->Properties["FILE"] = "0";
	item->Properties["PATH"] = "file:/";
	aItems.push_back(item);

	if(FTPEnumerator::GetEnabled())
	{
		SummerfaceItem* item = new SummerfaceItem("FTP Storage", "FolderICON");
		item->Properties["DIRECTORY"] = "1";
		item->Properties["FILE"] = "0";
		item->Properties["PATH"] = "ftp:/";
		aItems.push_back(item);
	}
}

std::string						EnumeratorEnumerator::ObtainFile			(const std::string& aPath)
{
	throw FileException("EnumeratorEnumerator::ObtainFile: EnumeratorEnumerator can't be used to get files.");
}

