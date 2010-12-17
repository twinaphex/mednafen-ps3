#include <ps3_system.h>

								FileListItem::FileListItem						(const std::string& aName, const std::string& aPath, bool aDirectory, bool aBookMark) : ListItem(aName, 0, aDirectory ? "FolderICON" : "FileICON")
{
	Path = aPath;

	Directory = aDirectory;
	File = !aDirectory;
	
	SetBookMark(aBookMark);
	
	if(ImageManager::GetImage(Utility::GetExtension(aPath) + "ICON"))
	{
		LabelImage = Utility::GetExtension(aPath) + "ICON";
	}
}

								FileListItem::FileListItem						(const std::string& aName, const std::string& aPath, struct ftpparse aData) : ListItem(aName)
{
	Path = aPath;

	File = aData.flagtryretr;
	Directory = aData.flagtrycwd;
	
	SetBookMark(false);
	
	if(ImageManager::GetImage(Utility::GetExtension(aPath) + "ICON"))
	{
		LabelImage = Utility::GetExtension(aPath) + "ICON";
	}
}

								FileListItem::~FileListItem						()
{
}

std::string						FileListItem::GetPath							()
{
	return Path;
}

bool							FileListItem::IsLink							()
{
	return File && Directory;
}

bool							FileListItem::IsDirectory						()
{
	return !File && Directory;
}

bool							FileListItem::IsFile							()
{
	return File && !Directory;
}

void							FileListItem::SetBookMark						(bool aSet)
{
	BookMark = aSet;
	TextColor = aSet ? Colors::SpecialNormal : Colors::Normal;
	SelectedTextColor = aSet ? Colors::SpecialHighLight : Colors::HighLight;
}

bool							FileListItem::GetBookMark						()
{
	return BookMark;
}
