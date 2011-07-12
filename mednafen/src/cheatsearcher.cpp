#include <mednafen_includes.h>

#include "cheatsearcher.h"

int							CheatSearcher::DoSearchFilterMenu		()
{
	if(!SearchFilterMenu)
	{
		SummerfaceList_Ptr list = smartptr::make_shared<SummerfaceList>(Area(10, 10, 80, 80));
		list->SetView(smartptr::make_shared<AnchoredListView>(list, true));
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

	SummerfaceList_Ptr plist = smartptr::static_pointer_cast<SummerfaceList>(SearchFilterMenu->GetWindow("TYPES"));
	return plist->WasCanceled() ? -1 : plist->GetSelection() * 100;
}

bool						CheatSearcher::GetNumber				(int64_t& aValue, const char* aHeader, uint32_t aDigits)
{
	SummerfaceNumber_Ptr number = smartptr::make_shared<SummerfaceNumber>(Area(10, 45, 80, 10), aValue, aDigits);
	number->SetHeader(aHeader);
	Summerface::Create("NUMB", number)->Do();

	if(!number->WasCanceled())
	{
		aValue = number->GetValue();
		return true;
	}

	return false;
}

bool						CheatSearcher::DoResultList				()
{
	//Get the cheat results
	Results.clear();
	MDFNI_CheatSearchGet(GetResults, 0);

	//Build the list
	SummerfaceList_Ptr list = smartptr::make_shared<SummerfaceList>(Area(10, 10, 80, 80));
	list->SetView(smartptr::make_shared<AnchoredListView>(list, true));
	list->SetHeader("Next Step: Choose a cheat, if any, that you want to install.");

	for(ResultList::iterator i = Results.begin(); i != Results.end(); i ++)
	{
		std::stringstream name;
		name << std::hex << i->Address << std::dec << " " << i->Original << " " << i->Changed;
		list->AddItem(smartptr::make_shared<SummerfaceItem>(name.str(), ""));
	}

	Summerface::Create("TYPES", list)->Do();

	//If the list was not canceld
	if(!list->WasCanceled())
	{
		const Result& item = Results.at(list->GetSelection());
		MDFNI_AddCheat("TEST", item.Address, 12, 0, 'R', 1, false);
	}

	return false;
}

void						CheatSearcher::Do						()
{
	//Get the search type
	if(State == -1)
	{
		State = DoSearchFilterMenu();
	}

	//Handle the search type
	if(State == -1)
	{
		//Cancled
		return;
	}
	
	if(State == 0 || State == 100 || State == 200 || State == 300 || State == 400 || State == 500)
	{
		//Get the original value
		if(State <= 100)
		{
			if(!GetNumber(Original, "Next Step: Enter Original Value"))
			{
				State = -1;
				return;
			}
		}
		else
		{
			Original = 0;
		}

		//Inform
		ESSUB_Error("Next Step: Come back when the value has changed");
		MDFNI_CheatSearchBegin();
		State ++;
		return;
	}
	else if(State == 1 || State == 101 || State == 201 || State == 301 || State == 401 || State == 501)
	{
		if(State <= 201)
		{
			//Get the changed value
			if(!GetNumber(Changed, "Next Step: Enter Changed Value"))
			{
				State = -1;
				return;
			}
		}
		else
		{
			Changed = 0;
		}

		//Get the byte length
		int64_t bytelen;
		if(!GetNumber(bytelen, "Next Step: How many bytes are in this value?", 1))
		{
			State = -1;
			return;
		}

		//Search
		MDFNI_CheatSearchEnd(State / 100, Original, Changed, bytelen, false);

		//Inform
		DoResultList();

		//Reset
		Reset();

		return;
	}
}

void						CheatSearcher::Reset					()
{
	State = -1;
}

CheatSearcher::ResultList	CheatSearcher::Results;
int32_t						CheatSearcher::State = -1;
Summerface_Ptr				CheatSearcher::SearchFilterMenu;
int64_t						CheatSearcher::Original;
int64_t						CheatSearcher::Changed;

