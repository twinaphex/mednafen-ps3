#include <ps3_system.h>

											ArchiveList::ArchiveList					(std::string aHeader, std::string aFileName, MenuHook* aInputHook) : WinterfaceList(aHeader, true, true, aInputHook)
{
	FileName = aFileName;

	if(0 != fex_open(&Archive, aFileName.c_str()))
	{
		throw "ArchiveList::ArchiveList: Fex could not open archive";
	}

	while(!fex_done(Archive))
	{
		if(fex_name(Archive) != 0)
		{
			Items.push_back(new ListItem(fex_name(Archive)));
		}
		
		fex_next(Archive);
	}
	
	SideItems.push_back(new ListItem("[DPAD] Navigate", FontManager::GetSmallFont()));
	SideItems.push_back(new ListItem("[X] Select Item", FontManager::GetSmallFont()));	
	SideItems.push_back(new ListItem("[O] Previous Dir", FontManager::GetSmallFont()));	
	SideItems.push_back(new ListItem("[R3] Settings", FontManager::GetSmallFont()));	 //HACK: Only with the mednafen hook!!!!
	SideItems.push_back(new ListItem("[START] Show Readme", FontManager::GetSmallFont()));	 //HACK: Only with the mednafen hook!!!!	
}

											ArchiveList::~ArchiveList					()
{
	fex_close(Archive);											
}

uint32_t									ArchiveList::ItemCount						()
{
	return Items.size();
}

uint32_t									ArchiveList::GetSelectedSize				()
{
	fex_rewind(Archive);
	
	while(!fex_done(Archive))
	{
		if(GetSelected()->GetText() == fex_name(Archive))
		{
			fex_stat(Archive);
			return fex_size(Archive);
		}
		
		fex_next(Archive);
	}
	
	throw "ArchiveList::GetSelectedSize: Fex could not find file in archive";
}

void										ArchiveList::GetSelectedData				(uint32_t aSize, void* aData)
{
	fex_rewind(Archive);

	while(!fex_done(Archive))
	{
		if(GetSelected()->GetText() == fex_name(Archive))
		{
			fex_read(Archive, aData, aSize);
			return;
		}
		
		fex_next(Archive);
	}
	
	throw "ArchiveList::GetSelectedData: Fex could not find file in archive";	
}

std::string									ArchiveList::GetSelectedFileName			()
{
	return GetSelected()->GetText();
}

bool										ArchiveList::IsArchive						(std::string aFileName)
{
	fex_type_t out;
	fex_identify_file(&out, aFileName.c_str());

	const char* tname = fex_type_name(out);
	
	if(tname && strcmp(tname, "file") == 0)
	{
		return false;
	}

	return true;
}