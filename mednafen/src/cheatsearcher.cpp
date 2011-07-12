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
	return plist->WasCanceled() ? -1 : plist->GetSelection();
}

bool						CheatSearcher::GetNumber				(int64_t& aValue, const char* aHeader)
{
	SummerfaceNumber_Ptr number = smartptr::make_shared<SummerfaceNumber>(Area(10, 10, 80, 80), aValue, 10);
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

	for(ResultList::iterator i = Results.begin(); i != Results.end(); i ++)
	{
		std::stringstream name;
		name << std::hex << i->Address << std::dec << " " << i->Original << " " << i->Changed;
		list->AddItem(smartptr::make_shared<SummerfaceItem>(name.str(), ""));
	}

	Summerface::Create("TYPES", list)->Do();
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
	
	if(State == 0)	//"Original == V1 && Changed == V2",
	{
		//Get the original value
		if(!GetNumber(Original, "Step 2: Enter Original Value"))
		{
			State = -1;
			return;
		}

		//Inform
		ESSUB_Error("Step 3: Come back when the value has changed");
		MDFNI_CheatSearchBegin();
		State ++;
	}
	else if(State == 1)
	{
		//Get the changed value
		if(!GetNumber(Changed, "Step 4: Enter Changed Value"))
		{
			State = -1;
			return;
		}

		//Search
		MDFNI_CheatSearchEnd(0, Original, Changed, 1, false);

		//Inform
		DoResultList();

		//Reset
		Reset();
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

