#include <ps3_system.h>
	
											WinterfaceList::WinterfaceList						(const std::string& aHeader, bool aCanPage, bool aKillItems, MenuHook* aHook) : Winterface(aHeader, aKillItems, aHook)
{
	Selected = 0;
	Canceled = 0;

	LinesDrawn = 0;
	CanPage = aCanPage;
}

bool										WinterfaceList::DrawLeft							()
{
	if(Items.size() == 0)
	{
		return false;
	}

	//TODO: Assume all items are the save size as item zero
	uint32_t itemheight = Items[0]->GetHeight();
	LinesDrawn = PS3Video::GetClip().Height / itemheight;
	
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

	return false;
}

bool										WinterfaceList::Input								()
{
	if(Items.size() == 0)
	{
		return false;
	}

	Selected += (PS3Input::ButtonPressed(0, PS3_BUTTON_DOWN) ? 1 : 0);
	Selected -= (PS3Input::ButtonPressed(0, PS3_BUTTON_UP) ? 1 : 0);
	
	if(CanPage)
	{
		Selected += (PS3Input::ButtonPressed(0, PS3_BUTTON_RIGHT) ? LinesDrawn : 0);
		Selected -= (PS3Input::ButtonPressed(0, PS3_BUTTON_LEFT) ? LinesDrawn : 0);
	}
	
	Selected = Utility::Clamp(Selected, 0, Items.size() - 1);
	
	if(PS3Input::ButtonDown(0, PS3_BUTTON_CIRCLE))
	{
		Canceled = true;
		return true;
	}
	
	if(Items[Selected]->Input())
	{
		Canceled = false;
		return true;
	}

	return Winterface::Input();
}
		