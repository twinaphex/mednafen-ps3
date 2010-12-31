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
	Canceled = false;
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
	XSelection += es_input->ButtonDown(0, ES_BUTTON_RIGHT) ? 1 : 0;
	XSelection -= es_input->ButtonDown(0, ES_BUTTON_LEFT) ? 1 : 0;
	XSelection = Utility::Clamp(XSelection, 0, (int32_t)Width - 1);

	YSelection += es_input->ButtonDown(0, ES_BUTTON_DOWN) ? 1 : 0;
	YSelection -= es_input->ButtonDown(0, ES_BUTTON_UP) ? 1 : 0;
	YSelection = Utility::Clamp(YSelection, 0, (int32_t)Height - 1);

	SelectedIndex = YSelection * Width + XSelection;

	if(es_input->ButtonDown(0, ES_BUTTON_CANCEL))
	{
		Canceled = true;
		return true;
	}

	if(SelectedIndex < Items.size() && Items[SelectedIndex]->Input())
	{
		Canceled = false;
		return true;
	}

	//TODO: What is Canceled's state if Winterface::Input returns true?
	return Winterface::Input();
}


bool								WinterfaceIconGrid::DrawLeft							()
{
	uint32_t iconWidth = es_video->GetClip().Width / Width - 4;
	uint32_t iconHeight = es_video->GetClip().Height / Height - 4;	

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

bool								WinterfaceIconGrid::WasCanceled							()
{
	return Canceled;
}

