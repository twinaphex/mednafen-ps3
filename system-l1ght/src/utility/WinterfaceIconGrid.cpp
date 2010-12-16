#include <ps3_system.h>

									WinterfaceIconGrid::WinterfaceIconGrid					(const std::string& aHeader, uint32_t aWidth, uint32_t aHeight, bool aKillItems) : Winterface(aHeader, aKillItems)
{
	if(aWidth == 0 || aHeight == 0 || aWidth > 16 || aHeight > 16)
	{
		throw "WinterfaceIconGrid::WinterfaceIconGrid: Invalid dimensions.";
	}

	Width = aWidth;
	Height = aHeight;
	
	XSelection = 0;
	YSelection = 0;
}

bool								WinterfaceIconGrid::DrawLeft							()
{
	uint32_t iconWidth = PS3Video::GetClip().Width / Width - 4;
	uint32_t iconHeight = PS3Video::GetClip().Height / Height - 4;	

	for(int i = 0; i != Height; i ++)
	{
		for(int j = 0; j != Width; j ++)
		{
			Items[i * Width + j]->Draw(j * iconWidth + 4, i * iconHeight + 4, iconWidth, iconHeight, j == XSelection && i == YSelection);
		}
	}
	
	return false;
}
