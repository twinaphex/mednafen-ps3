#include <ps3_system.h>

											ArchiveList::ArchiveList					(const Area& aRegion, const std::string& aFileName) : SummerfaceList(aRegion)
{
	FileName = aFileName;

	if(0 != fex_open(&Archive, aFileName.c_str()))
	{
		throw ESException("ArchiveList::ArchiveList: Fex could not open archive [File: %s]", aFileName.c_str());
	}

	while(!fex_done(Archive))
	{
		if(fex_name(Archive) != 0)
		{
			Items.push_back(new SummerfaceItem(fex_name(Archive), ""));
		}
		
		fex_next(Archive);
	}

	Sort();
}

											ArchiveList::~ArchiveList					()
{
	fex_close(Archive);		
}

uint32_t									ArchiveList::GetSelectedSize				()
{
	FindFexFile(GetSelected()->GetText());

	fex_stat(Archive);
	return fex_size(Archive);
}

void										ArchiveList::GetSelectedData				(uint32_t aSize, void* aData)
{
	FindFexFile(GetSelected()->GetText());

	//TODO: Throw on error
	fex_read(Archive, aData, aSize);
}

std::string									ArchiveList::GetSelectedFileName			()
{
	return GetSelected()->GetText();
}

void										ArchiveList::FindFexFile					(const std::string& aFileName)
{
	fex_rewind(Archive);

	while(!fex_done(Archive))
	{
		if(aFileName == fex_name(Archive))
		{
			return;
		}
		
		fex_next(Archive);
	}

	throw ESException("ArchiveList: Fex could not find file in archive [File: %s]", aFileName.c_str());
}

bool										ArchiveList::IsArchive						(const std::string& aFileName)
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

