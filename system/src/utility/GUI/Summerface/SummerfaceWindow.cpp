#include <ps3_system.h>
#include "SummerfaceWindow.h"

uint32_t ptpp(uint32_t aIn, bool aX)
{
	uint32_t s = aX ? es_video->GetScreenWidth() : es_video->GetScreenHeight();
	return s * ((double)aIn / 100.0);
}


											SummerfaceWindow::SummerfaceWindow					(const Area& aRegion)
{
	Interface = 0;
	InputHandler = 0;
	DeleteHandler = false;

	Area outregion(ptpp(aRegion.X, 1), ptpp(aRegion.Y, 0), ptpp(aRegion.Width, 1), ptpp(aRegion.Height, 0));

	Region = outregion;
	Client = Area(outregion.X + BorderWidth, outregion.Y + BorderWidth, outregion.Width - BorderWidth * 2, outregion.Height - BorderWidth * 2);
}

											SummerfaceWindow::~SummerfaceWindow					()
{
	if(InputHandler && DeleteHandler)
	{
		delete InputHandler;
	}
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

	return Draw();
}


bool										SummerfaceWindow::Input								()
{
	if(InputHandler)
	{
		return InputHandler->HandleInput(GetInterface(), GetName());
	}

	return false;
}

void										SummerfaceWindow::SetInterface						(Summerface* aInterface, const std::string& aName)
{
	if(!Interface)
	{
		Interface = aInterface;
		Name = aName;
	}
	else
	{
		throw ESException("SummerfaceWindow::SetInterface may only be called once per window. Once it's assigned it belongs to its parent forever.");
	}
}

Summerface*									SummerfaceWindow::GetInterface						()
{
	return Interface;
}

void										SummerfaceWindow::SetName							(const std::string& aName)
{
	Name = aName;
}

std::string									SummerfaceWindow::GetName							()
{
	return Name;
}


void										SummerfaceWindow::SetInputConduit					(SummerfaceInputConduit* aInputConduit, bool aDelete)
{
	if(InputHandler && DeleteHandler)
	{
		delete InputHandler;
	}

	InputHandler = aInputConduit;
	DeleteHandler = aDelete;
}

SummerfaceInputConduit*						SummerfaceWindow::GetInputConduit					()
{
	return InputHandler;
}


