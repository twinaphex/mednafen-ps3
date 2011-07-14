#include <mednafen_includes.h>

#include "cheatsearcher.h"

int							CheatSearcher::DoSearchFilterMenu		()
{
	if(!SearchFilterMenu)
	{
		smartptr::shared_ptr<AnchoredListView<SummerfaceItem> > list = smartptr::make_shared<AnchoredListView<SummerfaceItem> >(Area(10, 10, 80, 80));
		list->SetHeader("Step 1: Choose Cheat Search Type");

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
			list->AddItem(smartptr::make_shared<SummerfaceItem>(types[i], ""));
		}

		SearchFilterMenu = Summerface::Create("TYPES", list);
	}

	SearchFilterMenu->Do();

	smartptr::shared_ptr<AnchoredListView<SummerfaceItem> > plist = smartptr::static_pointer_cast<AnchoredListView<SummerfaceItem> >(SearchFilterMenu->GetWindow("TYPES"));
	return plist->WasCanceled() ? -1 : plist->GetSelection() * 100;
}

bool						CheatSearcher::GetNumber				(int64_t& aValue, const char* aHeader, uint32_t aDigits)
{
	SummerfaceNumber_Ptr number = smartptr::make_shared<SummerfaceNumber>(Area(10, 45, 80, 10), aValue, aDigits, false);
	number->SetHeader(aHeader);
	Summerface::Create("NUMB", number)->Do();

	if(!number->WasCanceled())
	{
		aValue = number->GetValue();
		return true;
	}

	return false;
}

bool						CheatSearcher::DoResultList				(uint32_t aBytes, bool aBigEndian)
{
	//Get the cheat results
	Results.clear();
	MDFNI_CheatSearchGet(GetResults, 0);

	//Build the list
	smartptr::shared_ptr<AnchoredListView<SummerfaceItem> > list = smartptr::make_shared<AnchoredListView<SummerfaceItem> >(Area(10, 10, 80, 80));
	list->SetHeader("Next Step: Choose a cheat, if any, that you want to install.");

	for(ResultList::iterator i = Results.begin(); i != Results.end(); i ++)
	{
		std::stringstream name;
		name << std::hex << i->Address << std::dec << " " << i->Original << " " << i->Changed;
		list->AddItem(smartptr::make_shared<SummerfaceItem>(name.str(), ""));
	}

	Summerface::Create("TYPES", list)->Do();

	//If the list was not canceled
	if(!list->WasCanceled())
	{
		const Result& item = Results.at(list->GetSelection());

		int64_t value = 0;
		while(GetNumber(value, "Enter value to patch to (in decimal)", 10))
		{
			std::string name = ESSUB_GetString("Enter name for the cheat", "");
			if(!name.empty())
			{
				MDFNI_AddCheat(name.c_str(), item.Address, value, 0, 'R', aBytes, aBigEndian);
				return false;
			}
		}
	}

	return false;
}

void						CheatSearcher::Do						()
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
			if(!GetNumber(Original, "Next Step: Enter Original Value"))
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
		if(State <= 2)
		{
			//Get the value, but leave if canceled
			if(!GetNumber(Changed, "Next Step: Enter Changed Value"))
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
		while(GetNumber(bytes, "Enter number of bytes to patch", 1))
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

void						CheatSearcher::Reset					()
{
	Mode = -1;
	State = -1;
}

CheatSearcher::ResultList	CheatSearcher::Results;
int32_t						CheatSearcher::Mode = -1;
int32_t						CheatSearcher::State = 0;
Summerface_Ptr				CheatSearcher::SearchFilterMenu;
int64_t						CheatSearcher::Original;
int64_t						CheatSearcher::Changed;

