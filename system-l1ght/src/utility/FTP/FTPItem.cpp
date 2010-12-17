#include <ps3_system.h>

								FTPItem::FTPItem								(const char* aName, const char* aPath, struct ftpparse aData) : ListItem(aName)
{
	Path = strdup(aPath);

	File = aData.flagtryretr;
	Directory = aData.flagtrycwd;
	
	SetImage(Directory ? "FolderICON" : "FileICON");
}

								FTPItem::~FTPItem								()
{
	free(Path);
}

const char*						FTPItem::GetFileName							()
{
	return GetText();
}
	
const char*						FTPItem::GetPath								()
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