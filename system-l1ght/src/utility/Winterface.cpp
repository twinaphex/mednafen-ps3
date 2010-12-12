#include <ps3_system.h>

namespace
{
	uint32_t wimage = 0;
};

											Winterface::Winterface								(std::string aHeader, bool aKillItems, MenuHook* aHook) : Menu(aHook)
{
	KillItems = aKillItems;
	Header = aHeader;
}


bool										Winterface::Draw									()
{
	//HACK:
	if(wimage == 0)
	{
		ImageManager::LoadDirectory("/dev_hdd0/game/MDFN90002/USRDIR/assets/png/");
		wimage = 1;
	}

	uint32_t screenW = PS3Video::GetScreenWidth();
	uint32_t screenH = PS3Video::GetScreenHeight();

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

	PS3Video::SetClip(Area(0, 0, screenW, screenH));

	PS3Video::FillRectangle(Area(0, 0, screenW - 1, screenH - 1), Colors::Border);

	if(ImageManager::GetImage("Background"))
	{
		//TODO: Fix for clipping
		PS3Video::PlaceTexture(ImageManager::GetImage("Background"), 0, 0, screenW - 1, screenH - 1, 0xFFFFFFFF);
	}

	PS3Video::FillRectangle(top, Colors::BackGround);
	PS3Video::FillRectangle(left, Colors::BackGround);	

	PS3Video::SetClip(top);
	FontManager::GetBigFont()->PutString(GetHeader(), 2, 0, Colors::Normal);
	
	PS3Video::SetClip(left);
	if(DrawLeft())
	{
		return true;
	}

	if(!HideRight)
	{
		PS3Video::SetClip(Area(0, 0, screenW, screenH));
		PS3Video::FillRectangle(right, Colors::BackGround);
	
		PS3Video::SetClip(right);
		return DrawRight();
	}
	
	return false;
}


bool										Winterface::HideRight = false;
