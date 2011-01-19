#include <ps3_system.h>

//TODO: Handle categories better, invalid ones should not crash or be available

											WinterfaceMultiList::WinterfaceMultiList			(const std::string& aHeader, bool aCanPage, bool aKillItems, MenuHook* aHook) : WinterfaceList(aHeader, aCanPage, false, aHook)
{
	MultiKillItems = aKillItems;
}

											WinterfaceMultiList::~WinterfaceMultiList			()
{
	if(MultiKillItems)
	{
		for(std::vector<ListItem*>::iterator iter = SideItems.begin(); iter != SideItems.end(); iter ++)
		{
			delete (*iter);
		}
		
		for(std::map<std::string, std::vector<ListItem*> >::iterator mapiter = Categories.begin(); mapiter != Categories.end(); mapiter ++)
		{
			for(std::vector<ListItem*>::iterator iter = mapiter->second.begin(); iter != mapiter->second.end(); iter ++)
			{
				delete (*iter);
			}
		}
	}									
}

bool										WinterfaceMultiList::Input							()
{
	std::string newCategory = CurrentCategory;

	if(es_input->ButtonDown(0, ES_BUTTON_AUXRIGHT1))
	{
		std::map<std::string, std::vector<ListItem*> >::iterator current = Categories.find(CurrentCategory);
		current ++;
		
		if(current == Categories.end())
		{
			newCategory = Categories.begin()->first;
		}
		else
		{
			newCategory = current->first;
		}
	}

	if(es_input->ButtonDown(0, ES_BUTTON_AUXLEFT1))
	{
		std::map<std::string, std::vector<ListItem*> >::iterator current = Categories.find(CurrentCategory);
		
		if(current == Categories.begin())
		{
			newCategory = (--Categories.end())->first;
		}
		else
		{
			newCategory = (--current)->first;
		}
	}
	
	SetCategory(newCategory);
	
	return WinterfaceList::Input();
}

void										WinterfaceMultiList::SetCategory					(const std::string& aCategory)
{
	if(aCategory != CurrentCategory)
	{
		Selected = 0;
		CurrentCategory = aCategory;
		
		Items = Categories[CurrentCategory];
	}
}


