#include <ps3_system.h>

//TODO: Handle categories better, invalid ones should not crash or be available

											WinterfaceMultiList::WinterfaceMultiList			(std::string aHeader, bool aCanPage, bool aKillItems, MenuHook* aHook) : WinterfaceList(aHeader, aCanPage, false, aHook)
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

	if(PS3Input::ButtonDown(0, PS3_BUTTON_R1))
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

	if(PS3Input::ButtonDown(0, PS3_BUTTON_L1))
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

