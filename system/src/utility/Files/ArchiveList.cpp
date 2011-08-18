#include <es_system.h>
#include "ArchiveList.h"
#include "src/thirdparty/fex/fex/fex.h"

											ArchiveList::ArchiveList					(const Area& aRegion, const std::string& aFileName) : 
	AnchoredListView<SummerfaceItem>(aRegion),
	Archive(0)
{
	//Open the archive
	if(0 == fex_open(&Archive, aFileName.c_str()))
	{
		//Populate the list
		while(!fex_done(Archive))
		{
			if(fex_name(Archive) != 0)
			{
				AddItem(smartptr::make_shared<SummerfaceItem>(fex_name(Archive), ""));
			}
		
			fex_next(Archive);
		}

		//Finish
		Sort();
	}
	//Add a default item
	else
	{
		Archive = 0;
		AddItem(smartptr::make_shared<SummerfaceItem>("FAILED TO OPEN ARCHIVE", "ErrorIMAGE"));
	}
}

											ArchiveList::~ArchiveList					()
{
	//Close any archives
	if(Archive)
	{
		fex_close(Archive);
	}
}

uint32_t									ArchiveList::GetSelectedSize				() const
{
	if(Archive)
	{
		FindFexFile(GetSelected()->GetText());

		fex_stat(Archive);
		return fex_size(Archive);
	}
	else
	{
		return 0;
	}
}

bool										ArchiveList::GetSelectedData				(uint32_t aSize, void* aData) const
{
	if(Archive)
	{
		FindFexFile(GetSelected()->GetText());

		if(0 == fex_read(Archive, aData, aSize))
		{
			return true;
		}
	}

	return false;
}

bool										ArchiveList::IsArchive						(const std::string& aFileName)
{
	fex_type_t out;
	fex_identify_file(&out, aFileName.c_str());

	const char* tname = fex_type_name(out);

	if(!tname || (tname && strcmp(tname, "file")) == 0)
	{
		return false;
	}

	return true;
}

bool										ArchiveList::FindFexFile					(const std::string& aFileName) const
{
	assert(Archive);

	fex_rewind(Archive);

	while(!fex_done(Archive))
	{
		if(aFileName == fex_name(Archive))
		{
			return true;
		}
		
		fex_next(Archive);
	}

	return false;
}

