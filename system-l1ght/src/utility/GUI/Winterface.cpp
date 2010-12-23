#include <ps3_system.h>

namespace
{
	uint32_t wimage = 0;
};

											Winterface::Winterface								(const std::string& aHeader, bool aKillItems, MenuHook* aHook) : Menu(aHook)
{
	Header = aHeader;
	
	KillItems = aKillItems;
}

											Winterface::~Winterface								()
{
	if(KillItems)
	{
		for(std::vector<ListItem*>::iterator iter = SideItems.begin(); iter != SideItems.end(); iter ++)
		{
			delete (*iter);
		}
	}
}

bool										Winterface::Input									()
{
	if(ESInput::ButtonDown(0, ES_BUTTON_AUXRIGHT2))
	{
		HideRight = !HideRight;
	}
	
	return false;
}

bool										Winterface::Draw									()
{
	//HACK:
	if(wimage == 0)
	{
		ImageManager::LoadDirectory("/dev_hdd0/game/MDFN90002/USRDIR/assets/png/");
		wimage = 1;
	}

	uint32_t screenW = ESVideo::GetScreenWidth();
	uint32_t screenH = ESVideo::GetScreenHeight();

	uint32_t screenW10 = ((double)screenW) * .05;
	uint32_t screenH10 = ((double)screenH) * .05;
	uint32_t screenW75 = ((double)screenW) * .75;
	uint32_t screenW25 = ((double)screenW) * .25;

	Area left = Area(screenW10, screenH10 * 2 + 5, screenW75 - screenW10 - 5, screenH - screenH10 * 3);
	Area right = Area(screenW75 + 5, screenH10 * 2 + 5, screenW25 - screenW10 - 5, screenH - screenH10 * 3);
	Area top = Area(screenW10, screenH10, screenW - screenW10 * 2, screenH10);

	if(HideRight)
	{
		left = Area(screenW10, screenH10 * 2 + 5, screenW - (screenW10 * 2), screenH - screenH10 * 3);	
	}

	ESVideo::SetClip(Area(0, 0, screenW, screenH));

	ESVideo::FillRectangle(Area(0, 0, screenW - 1, screenH - 1), Colors::Border);

	if(ImageManager::GetImage("Background"))
	{
		//TODO: Fix for clipping
		ESVideo::PlaceTexture(ImageManager::GetImage("Background"), 0, 0, screenW - 1, screenH - 1, 0xFFFFFFFF);
	}

	ESVideo::FillRectangle(top, Colors::BackGround);
	ESVideo::FillRectangle(left, Colors::BackGround);	

	ESVideo::SetClip(top);
	FontManager::GetBigFont()->PutString(GetHeader(), 2, 0, Colors::Normal);
	
	ESVideo::SetClip(left);
	if(DrawLeft())
	{
		return true;
	}

	if(!HideRight)
	{
		ESVideo::SetClip(Area(0, 0, screenW, screenH));
		ESVideo::FillRectangle(right, Colors::BackGround);
	
		ESVideo::SetClip(right);
		return DrawRight();
	}
	
	return false;
}

bool										Winterface::DrawLeft								()
{
	return false;
}

bool										Winterface::DrawRight								()
{
	//TODO: Assume all items are the same size as item[0]
	for(int i = 0; i != SideItems.size(); i ++)
	{
		SideItems[i]->Draw(16, i * SideItems[0]->GetHeight(), false);
	}

	return false;
}

const char*									Winterface::GetHeader								()
{
	return Header.c_str();
}


bool										Winterface::HideRight = false;
