#include <es_system.h>
#include "Summerface.h"

											Summerface::Summerface								(const std::string& aName, SummerfaceWindow* aWindow)
{
	if(aWindow)
	{
		AddWindow(aName, aWindow);
	}
}

											Summerface::~Summerface								()
{
	//Kill InputConduits
	for(ConduitSet::iterator i = Handlers.begin(); i != Handlers.end(); i ++)
	{
		delete *i;
	}

	//Kill Windows
	for(WindowSet::iterator i = Windows.begin(); i != Windows.end(); i ++)
	{
		delete i->second;
	}
}

bool										Summerface::Draw									()
{
	//Get the screen size and reset the clip
	uint32_t screenW = ESVideo::GetScreenWidth();
	uint32_t screenH = ESVideo::GetScreenHeight();
	ESVideo::SetClip(Area(0, 0, screenW, screenH));

	//Draw the background
	bool used_background = BackgroundCallback && BackgroundCallback();

	//Search for a background image
	Texture* tex = ImageManager::GetImage(used_background ? "GUIOverlay" : "GUIBackground");

	//No backgound image, use a solid color
	if(!tex)
	{
		ESVideo::FillRectangle(Area(0, 0, screenW, screenH), used_background ? 0x00000080 : Colors::Border);
	}
	//Draw the backgound image
	else
	{
		ESVideo::PlaceTexture(tex, Area(0, 0, screenW, screenH), Area(0, 0, tex->GetWidth(), tex->GetHeight()), 0xFFFFFF80);
	}

	//Draw the windows
	for(WindowSet::iterator i = Windows.begin(); i != Windows.end(); i ++)
	{
		if(i->second->PrepareDraw())
		{
			//Window said to stop processing
			return true;
		}
	}

	//Continue processing
	return false;
}

bool										Summerface::Input									(uint32_t aButton)
{
	assert(Windows[ActiveWindow]);

	//Check for conduits
	for(ConduitSet::iterator i = Handlers.begin(); i != Handlers.end(); i ++)
	{
		if(*i)
		{
			int result = (*i)->HandleInput(this, ActiveWindow, aButton);

			if(result)
			{
				return (result > 0) ? false : true;
			}
		}
	}

	return Windows[ActiveWindow]->Input(aButton);
}

bool										(*Summerface::BackgroundCallback)					() = 0;

