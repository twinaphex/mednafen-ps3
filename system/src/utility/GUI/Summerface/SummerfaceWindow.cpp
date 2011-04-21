#include <es_system.h>
#include "SummerfaceWindow.h"

uint32_t ptpp(uint32_t aIn, bool aX)
{
	uint32_t s = aX ? es_video->GetScreenWidth() : es_video->GetScreenHeight();
	return s * ((double)aIn / 100.0);
}


											SummerfaceWindow::SummerfaceWindow					(const Area& aRegion) :
	Interface(Summerface_WeakPtr()),
	InputHandler(SummerfaceInputConduit_Ptr())
{
	Region = Area(ptpp(aRegion.X, 1), ptpp(aRegion.Y, 0), ptpp(aRegion.Width, 1), ptpp(aRegion.Height, 0));;
	Client = Area(Region.X + BorderWidth, Region.Y + BorderWidth, Region.Width - BorderWidth * 2, Region.Height - BorderWidth * 2);
}

//Draw the border and background, leave clip set to the windows client area
bool										SummerfaceWindow::PrepareDraw						()
{
	es_video->SetClip(Area(0, 0, es_video->GetScreenWidth(), es_video->GetScreenHeight()));

//TODO: Make border color
	es_video->FillRectangle(Area(Region.X, Region.Y, Region.Width, BorderWidth), 0xFFFFFFFF);
	es_video->FillRectangle(Area(Region.X, Region.Bottom() - BorderWidth, Region.Width, BorderWidth), 0xFFFFFFFF);

	es_video->FillRectangle(Area(Region.X, Region.Y + BorderWidth, BorderWidth, Region.Height - BorderWidth * 2), 0xFFFFFFFF);
	es_video->FillRectangle(Area(Region.Right() - BorderWidth, Region.Y + BorderWidth, BorderWidth, Region.Height - BorderWidth * 2), 0xFFFFFFFF);

	es_video->FillRectangle(Client, Colors::BackGround);
	es_video->SetClip(Client);

	std::string header = GetHeader();
	if(!header.empty())
	{
		FontManager::GetBigFont()->PutString(header.c_str(), 1, 1, Colors::Normal);
		es_video->FillRectangle(Area(0, FontManager::GetBigFont()->GetHeight() + 1, Client.Width, 1), 0xFFFFFFFF);
		es_video->SetClip(Area(Client.X, Client.Y + FontManager::GetBigFont()->GetHeight() + 3, Client.Width, Client.Height - (FontManager::GetBigFont()->GetHeight() + 3)));
	}

	return Draw();
}


bool										SummerfaceWindow::Input								()
{
	if(InputHandler)
	{
		return InputHandler->HandleInput(GetInterface(), GetName());
	}
	else
	{
		return es_input->ButtonDown(0, ES_BUTTON_CANCEL);
	}
}

void										SummerfaceWindow::SetInterface						(Summerface_Ptr aInterface, const std::string& aName)
{
	Interface = aInterface;
	Name = aName;
}

Summerface_Ptr								SummerfaceWindow::GetInterface						()
{
	if(!Interface.expired())
	{
		return Interface.lock();
	}

	return Summerface_Ptr();
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

