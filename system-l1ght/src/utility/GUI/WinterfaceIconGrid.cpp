#include <ps3_system.h>

									WinterfaceIconGrid::WinterfaceIconGrid					(const std::string& aHeader, uint32_t aWidth, uint32_t aHeight, bool aKillItems) : Winterface(aHeader, aKillItems)
{
	if(aWidth == 0 || aHeight == 0 || aWidth > 16 || aHeight > 16)
	{
		Abort("WinterfaceIconGrid::WinterfaceIconGrid: Invalid dimensions.");
	}

	Width = aWidth;
	Height = aHeight;
	
	XSelection = 0;
	YSelection = 0;
	SelectedIndex = 0;
}

									WinterfaceIconGrid::~WinterfaceIconGrid					()
{
	if(KillItems)
	{
		for(std::vector<GridItem*>::iterator iter = Items.begin(); iter != Items.end(); iter ++)
		{
			delete (*iter);
		}
	}
}

bool								WinterfaceIconGrid::Input								()
{
	XSelection += PS3Input::ButtonDown(0, PS3_BUTTON_RIGHT) ? 1 : 0;
	XSelection -= PS3Input::ButtonDown(0, PS3_BUTTON_LEFT) ? 1 : 0;
	XSelection = Utility::Clamp(XSelection, 0, (int32_t)Width - 1);

	YSelection += PS3Input::ButtonDown(0, PS3_BUTTON_DOWN) ? 1 : 0;
	YSelection -= PS3Input::ButtonDown(0, PS3_BUTTON_UP) ? 1 : 0;
	YSelection = Utility::Clamp(YSelection, 0, (int32_t)Height - 1);

	SelectedIndex = YSelection * Width + XSelection;

	if(SelectedIndex < Items.size() && Items[SelectedIndex]->Input())
	{
		return true;
	}

	return Winterface::Input();
}


bool								WinterfaceIconGrid::DrawLeft							()
{
	uint32_t iconWidth = PS3Video::GetClip().Width / Width - 4;
	uint32_t iconHeight = PS3Video::GetClip().Height / Height - 4;	

	for(int i = 0; i != Height; i ++)
	{
		for(int j = 0; j != Width; j ++)
		{
			if(i * Width + j >= Items.size())
			{
				break;
			}
		
			Items[i * Width + j]->Draw(j * iconWidth + 4, i * iconHeight + 4, iconWidth, iconHeight, j == XSelection && i == YSelection);
		}
	}
	
	return false;
}
