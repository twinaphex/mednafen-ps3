#include <ps3_system.h>

								FTPItem::FTPItem								(const std::string& aName, const std::string& aPath, struct ftpparse aData) : ListItem(aName)
{
	Path = aPath;

	File = aData.flagtryretr;
	Directory = aData.flagtrycwd;
	
	LabelImage = Directory ? "FolderICON" : "FileICON";
}

								FTPItem::~FTPItem								()
{
}

std::string						FTPItem::GetPath								()
{
	return Path;
}

bool							FTPItem::IsLink									()
{
	return File && Directory;
}

bool							FTPItem::IsDirectory							()
{
	return !File && Directory;
}

bool							FTPItem::IsFile									()
{
	return File && !Directory;
}