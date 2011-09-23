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

		//Four edges + two shadows, ugly
		Area borders[6] =
		{
			Area(pixRegion.X, 						pixRegion.Y,						pixRegion.Width,	BorderWidth),
			Area(pixRegion.X, 						pixRegion.Bottom() - BorderWidth,	pixRegion.Width,	BorderWidth),
			Area(pixRegion.X, 						pixRegion.Y, 						BorderWidth,		pixRegion.Height),
			Area(pixRegion.Right() - BorderWidth, 	pixRegion.Y, 						BorderWidth,		pixRegion.Height),
			Area(pixRegion.X + BorderWidth * 3,		pixRegion.Bottom(),					pixRegion.Width,	BorderWidth * 3),
			Area(pixRegion.Right(), 				pixRegion.Y + BorderWidth,			BorderWidth * 3,	pixRegion.Height - BorderWidth),
		};

		for(int i = 0; i != 6; i ++)
		{
			ESVideo::FillRectangle(borders[i], (i < 4) ? (uint32_t)BorderColor : 0x80);
		}

		//Remove border pixels from region and fill the inside of the window
		pixRegion.Inflate(-BorderWidth);
		ESVideo::FillRectangle(pixRegion, BackgroundColor);
		ESVideo::SetClip(pixRegion);

		//Draw the header
		if(!Header.empty())
		{
			uint32_t headerHeight = FontManager::GetBigFont()->GetHeight();

			//Draw the text with background
			ESVideo::FillRectangle(Area(0, 0, pixRegion.Width, headerHeight), HeaderColor);
			FontManager::GetBigFont()->PutString(Header.c_str(), MarginSize, 0, TextColor, true);

			//Draw a line separating the header from the window
			ESVideo::FillRectangle(Area(0, 0, headerHeight + 1, 1), BorderColor);

			//Adjust window size accordingly
			pixRegion.Y += headerHeight + 2;
			pixRegion.Height -= headerHeight + 2;
		}
	}

	pixRegion.Inflate(-MarginSize);
	ESVideo::SetClip(pixRegion);

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

