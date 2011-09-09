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
	UseBorder(aBorder),
	BackgroundColor("windowbackground", Colors::white),
	TextColor("text", Colors::black),
	HeaderColor("header", Colors::lightgreen),
	BorderColor("border", Colors::white)
{
}

//Draw the border and background, leave clip set to the windows client area
bool										SummerfaceWindow::PrepareDraw						()
{
	//Convert screen area from percent to pixels
	Area pixRegion = Area_PercentToPix(Region);

	if(UseBorder)
	{
		ESVideo::SetClip(Area(0, 0, ESVideo::GetScreenWidth(), ESVideo::GetScreenHeight()));

		ESVideo::FillRectangle(Area(pixRegion.X, pixRegion.Y, pixRegion.Width, BorderWidth), BorderColor);
		ESVideo::FillRectangle(Area(pixRegion.X + BorderWidth, pixRegion.Bottom(), pixRegion.Width, BorderWidth), 0x00000080);
		ESVideo::FillRectangle(Area(pixRegion.X, pixRegion.Bottom() - BorderWidth, pixRegion.Width, BorderWidth), BorderColor);

		ESVideo::FillRectangle(Area(pixRegion.X, pixRegion.Y + BorderWidth, BorderWidth, pixRegion.Height - BorderWidth * 2), BorderColor);
		ESVideo::FillRectangle(Area(pixRegion.Right(), pixRegion.Y + BorderWidth, BorderWidth, pixRegion.Height - BorderWidth), 0x00000080);
		ESVideo::FillRectangle(Area(pixRegion.Right() - BorderWidth, pixRegion.Y + BorderWidth, BorderWidth, pixRegion.Height - BorderWidth * 2), BorderColor);

		//Remove border pixels from region
		pixRegion.Inflate(-BorderWidth);
		ESVideo::FillRectangle(pixRegion, BackgroundColor);
		ESVideo::SetClip(pixRegion);

		std::string header = GetHeader();
		if(!header.empty())
		{
			ESVideo::FillRectangle(Area(0, 0, pixRegion.Width, FontManager::GetBigFont()->GetHeight() + 4), HeaderColor);
			FontManager::GetBigFont()->PutString(header.c_str(), 1, 0, TextColor, true);
			ESVideo::FillRectangle(Area(0, FontManager::GetBigFont()->GetHeight() + 4, pixRegion.Width, 1), 0xFFFFFFFF);
			ESVideo::SetClip(Area(pixRegion.X, pixRegion.Y + FontManager::GetBigFont()->GetHeight() + 4, pixRegion.Width, pixRegion.Height - (FontManager::GetBigFont()->GetHeight() + 4)));
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

