#include <es_system.h>
#include "SummerfaceWindow.h"

namespace
{
	uint32_t								ptpp												(uint32_t aIn, bool aX)
	{
		uint32_t s = aX ? ESVideo::GetScreenWidth() : ESVideo::GetScreenHeight();
		return s * ((double)aIn / 100.0);
	}

	Area									Area_PercentToPix									(const Area& aArea)
	{
		return Area(ptpp(aArea.X, 1), ptpp(aArea.Y, 0), ptpp(aArea.Width, 1), ptpp(aArea.Height, 0));
	}
}


											SummerfaceWindow::SummerfaceWindow					(const Area& aRegion, bool aBorder) :
	Interface(0),
	Region(aRegion),
	UseBorder(aBorder)
{
}

//Draw the border and background, leave clip set to the windows client area
bool										SummerfaceWindow::PrepareDraw						()
{
	//Convert screen area from percent to pixels
	Area pixRegion = Area_PercentToPix(Region);

	if(UseBorder)
	{
#if 0
		if(ImageManager::GetImage("Window"))
		{
			Texture* tex = ImageManager::GetImage("Window");
			tex->SetFilter(false);

			Area scorner = Area(Region.X, Region.Y, 8, 8);
			Area tcorner = Area(0, 0, 8, 8);
			ESVideo::PlaceTexture(tex, scorner, tcorner, 0xFFFFFFFF);

			scorner.X += Region.Width - 8;
			tcorner.X += tex->GetWidth() - 8;
			ESVideo::PlaceTexture(tex, scorner, tcorner, 0xFFFFFFFF);

			scorner.Y += Region.Height - 8;
			tcorner.Y += tex->GetHeight() - 8;
			ESVideo::PlaceTexture(tex, scorner, tcorner, 0xFFFFFFFF);

			scorner.X -= Region.Width - 8;
			tcorner.X -= tex->GetWidth() - 8;
			ESVideo::PlaceTexture(tex, scorner, tcorner, 0xFFFFFFFF);

			Area swidther = Area(Region.X + 8, Region.Y, Region.Width - 16, 8);
			Area twidther = Area(8, 0, tex->GetWidth() - 16, 8);
			ESVideo::PlaceTexture(tex, swidther, twidther, 0xFFFFFFFF);

			swidther.Y += Region.Height - 8;
			twidther.Y += tex->GetHeight() - 8;
			ESVideo::PlaceTexture(tex, swidther, twidther, 0xFFFFFFFF);

			swidther = Area(Region.X, Region.Y + 8, 8, Region.Height - 16);
			twidther = Area(0, 8, 8, tex->GetHeight() - 16);
			ESVideo::PlaceTexture(tex, swidther, twidther, 0xFFFFFFFF);

			swidther.X += Region.Width - 8;
			twidther.X += tex->GetWidth() - 8;
			ESVideo::PlaceTexture(tex, swidther, twidther, 0xFFFFFFFF);

			Area sinner = Area(Region.X + 8, Region.Y + 8, Region.Width - 16, Region.Height - 16);
			Area tinner = Area(8, 8, tex->GetWidth() - 16, tex->GetHeight() - 16);
			ESVideo::PlaceTexture(tex, sinner, tinner, 0xFFFFFFFF);

		}
		else
#endif
		{
			ESVideo::SetClip(Area(0, 0, ESVideo::GetScreenWidth(), ESVideo::GetScreenHeight()));

			//TODO: Make border color
			ESVideo::FillRectangle(Area(pixRegion.X, pixRegion.Y, pixRegion.Width, BorderWidth), 0xFFFFFFFF);
			ESVideo::FillRectangle(Area(pixRegion.X + BorderWidth, pixRegion.Bottom(), pixRegion.Width, BorderWidth), 0x00000080);
			ESVideo::FillRectangle(Area(pixRegion.X, pixRegion.Bottom() - BorderWidth, pixRegion.Width, BorderWidth), 0xFFFFFFFF);

			ESVideo::FillRectangle(Area(pixRegion.X, pixRegion.Y + BorderWidth, BorderWidth, pixRegion.Height - BorderWidth * 2), 0xFFFFFFFF);
			ESVideo::FillRectangle(Area(pixRegion.Right(), pixRegion.Y + BorderWidth, BorderWidth, pixRegion.Height - BorderWidth), 0x00000080);
			ESVideo::FillRectangle(Area(pixRegion.Right() - BorderWidth, pixRegion.Y + BorderWidth, BorderWidth, pixRegion.Height - BorderWidth * 2), 0xFFFFFFFF);

			//Remove border pixels from region
			pixRegion.Inflate(-BorderWidth);
			ESVideo::FillRectangle(pixRegion, Colors::BackGround);
		}

		ESVideo::SetClip(pixRegion);

		std::string header = GetHeader();
		if(!header.empty())
		{
			FontManager::GetBigFont()->PutString(header.c_str(), 1, 1, Colors::Normal, true);
			ESVideo::FillRectangle(Area(0, FontManager::GetBigFont()->GetHeight() + 1, pixRegion.Width, 1), 0xFFFFFFFF);
			ESVideo::SetClip(Area(pixRegion.X, pixRegion.Y + FontManager::GetBigFont()->GetHeight() + 3, pixRegion.Width, pixRegion.Height - (FontManager::GetBigFont()->GetHeight() + 3)));
		}
	}
	else
	{
		ESVideo::SetClip(pixRegion);
	}

	return Draw();
}

void										SummerfaceWindow::SetHeader							(const std::string& aHeader, ...)
{
	char array[1024];
	va_list args;
	va_start (args, aHeader);
	vsnprintf(array, 1024, aHeader.c_str(), args);
	va_end(args);

	Header = array;
}

