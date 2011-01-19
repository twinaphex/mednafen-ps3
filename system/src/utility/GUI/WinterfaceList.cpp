#include <ps3_system.h>
	
											WinterfaceList::WinterfaceList						(const std::string& aHeader, bool aCanPage, bool aKillItems, MenuHook* aHook) : Winterface(aHeader, aKillItems, aHook)
{
	Selected = 0;
	Canceled = 0;

	LinesDrawn = 0;
	CanPage = aCanPage;
}

											WinterfaceList::~WinterfaceList						()
{
	if(KillItems)
	{
		for(std::vector<ListItem*>::iterator i = Items.begin(); i != Items.end(); i ++)
		{
			delete *i;
		}
	}
}


bool										WinterfaceList::DrawLeft							()
{
	if(Items.size() != 0)
	{
		//TODO: Assume all items are the save size as item zero
		uint32_t itemheight = Items[0]->GetHeight();
		LinesDrawn = es_video->GetClip().Height / itemheight;
		
		//TODO: Fix it to draw one or two line lists!
		if(LinesDrawn < 3)
		{
			return true;
		}
		
		uint32_t online = 0;
	
		for(int i = Selected - LinesDrawn / 2; i != Selected + LinesDrawn / 2; i ++)
		{
			if(i < 0)
			{
				online ++;
				continue;
			}
		
			if(i >= Items.size())
			{
				break;
			}
			
			Items[i]->Draw(16, (online * itemheight), i == Selected);
	
			online ++;
		}
	}

	return false;
}

bool										WinterfaceList::Input								()
{
	if(Items.size() != 0)
	{
		Selected += (es_input->ButtonPressed(0, ES_BUTTON_DOWN) ? 1 : 0);
		Selected -= (es_input->ButtonPressed(0, ES_BUTTON_UP) ? 1 : 0);
	
		if(CanPage)
		{
			Selected += (es_input->ButtonPressed(0, ES_BUTTON_RIGHT) ? LinesDrawn : 0);
			Selected -= (es_input->ButtonPressed(0, ES_BUTTON_LEFT) ? LinesDrawn : 0);
		}
	
		Selected = Utility::Clamp(Selected, 0, Items.size() - 1);

		if(Items[Selected]->Input())
		{
			Canceled = false;
			return true;
		}
	}
		
	if(es_input->ButtonDown(0, ES_BUTTON_CANCEL))
	{
		Canceled = true;
		return true;
	}
	
	//TODO: What is Canceled's state if Winterface::Input returns true?
	return Winterface::Input();
}

uint32_t									WinterfaceList::GetItemCount						()
{
	return Items.size();
}

void										WinterfaceList::SetSelection						(uint32_t aIndex)
{
	if(aIndex >= Items.size())
	{
		throw ESException("WinterfaceList: Item index out of range [Item %d, Total %d]", aIndex, Items.size());
	}

	Selected = aIndex;
}

bool										WinterfaceList::WasCanceled							()
{
	return Canceled;
}

ListItem*									WinterfaceList::GetSelected							()
{
	if(Selected >= Items.size())
	{
		Abort("WinterfaceList::GetSelected: Selected was larger than the list, was the list empty?");
	}

	return Items[Selected];
}
