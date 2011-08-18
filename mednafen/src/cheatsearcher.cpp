#include <mednafen_includes.h>

#include "cheatsearcher.h"

int									CheatSearcher::DoSearchFilterMenu		()
{
	//Create the list if needed
	if(!SearchFilterList)
	{
		SearchFilterList = new ModeListType(Area(10, 10, 80, 80));
		SearchFilterList->SetHeader("Step 1: Choose Cheat Search Type");

		const char *const types[6] =
		{
			"Original == V1 && Changed == V2",
			"Original == V1 && (Original - Changed) == V2",
			"(Original - Changed) == V2",
			"Value changed",
			"Value decreased",
			"Value increased"
		};	

		for(int i = 0; i != 6; i ++)
		{
			SearchFilterList->AddItem(new SummerfaceItem(types[i], "", i));
		}
	}

	//Run it and return the result
	Summerface sface("TYPES", SearchFilterList); sface.Do();
	return SearchFilterList->WasCanceled() ? -1 : SearchFilterList->GetSelected()->UserData;
}

bool								CheatSearcher::DoResultList				(uint32_t aBytes, bool aBigEndian)
{
	//Create a list if needed
	if(!ResultList)
	{
		ResultList = new ResultListType(Area(10, 10, 80, 80));
		ResultList->SetHeader("Next Step: Choose a cheat, if any, that you want to install.");
	}

	//If there are any results, show them
	if(MDFNI_CheatSearchGetCount() > 0)
	{
		//Get the cheat results
		ResultList->ClearItems();
		MDFNI_CheatSearchGet(GetResults, 0);

		//Run the list
		Summerface sface("TYPES", ResultList); sface.Do();

		//If the list was not canceled
		if(!ResultList->WasCanceled())
		{
			//Get a value for the cheat
			int64_t value = 0;
			while(ESSUB_GetNumber(value, "Enter value to patch to (in decimal)", 10, false))
			{
				//Get a name for the cheat
				std::string name = ESSUB_GetString("Enter name for the cheat", "");
				if(!name.empty())
				{
					MDFNI_AddCheat(name.c_str(), ResultList->GetSelected()->UserData.Address, value, 0, 'R', aBytes, aBigEndian);
					return false;
				}
			}
		}
	}
	else
	{
		ESSUB_Error("Search produced no results.");
		return false;
	}

	return false;
}

void								CheatSearcher::Do						()
{
	//Get the search type
	if(Mode == -1)
	{
		if((Mode = DoSearchFilterMenu()) == -1)
		{
			//Search filter menu canceled
			return;
		}
	}
	
	//Get the first value
	if(0 == State)
	{
		//Get an original value if needed
		if(Mode <= 1)
		{
			//Get the value, but leave if canceled
			if(!ESSUB_GetNumber(Original, "Next Step: Enter Original Value", 10, false))
			{
				Reset();
				return;
			}
		}
		else
		{
			Original = 0;
		}

		//Reset the cheat search and tell the user to come back later!
		MDFNI_CheatSearchBegin();
		ESSUB_Error("Next Step: Come back when the value has changed");
		State ++;
		return;
	}
	else if(State == 1)
	{
		//Get a changed value if needed
		if(Mode <= 2)
		{
			//Get the value, but leave if canceled
			if(!ESSUB_GetNumber(Changed, "Next Step: Enter Changed Value", 10, false))
			{
				return;
			}
		}
		else
		{
			Changed = 0;
		}

		//Get the byte length
		int64_t bytes = 0;
		while(ESSUB_GetNumber(bytes, "Enter number of bytes to patch", 1, false))
		{
			//Handle case where bytes is invalid
			if(bytes == 0 || bytes > 8)
			{
				ESSUB_Error("Bytes value must be between 1 and 8 inclusive.");
			}
			else
			{
				//Get the endian state if needed
				bool bigendian = (bytes == 1) ? false : ESSUB_Confirm("Is memory big-endian?");

				//End the search
				MDFNI_CheatSearchEnd(State / 100, Original, Changed, bytes, bigendian);
				DoResultList(bytes, bigendian);
				Reset();

				//Leave
				return;
			}
		}

		return;
	}
}

void								CheatSearcher::Reset					()
{
	Mode = -1;
	State = 0;
}

CheatSearcher::ResultListType*		CheatSearcher::ResultList;
int32_t								CheatSearcher::Mode = -1;
int32_t								CheatSearcher::State = 0;
CheatSearcher::ModeListType*		CheatSearcher::SearchFilterList;
int64_t								CheatSearcher::Original;
int64_t								CheatSearcher::Changed;

