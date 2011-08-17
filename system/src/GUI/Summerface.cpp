#include <es_system.h>
#include "Summerface.h"

Summerface_Ptr								Summerface::Create									()
{
	Summerface_Ptr sface = smartptr::make_shared<Summerface>();
	assert(sface);
	return sface;
}


Summerface_Ptr								Summerface::Create									(const std::string& aName, SummerfaceWindow_Ptr aWindow)
{
	Summerface_Ptr sface = smartptr::make_shared<Summerface>();
	sface->AddWindow(aName, aWindow);

	assert(sface);

	return sface;
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
	for(std::map<std::string, SummerfaceWindow_Ptr>::iterator i = Windows.begin(); i != Windows.end(); i ++)
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
			int result = (*i)->HandleInput(shared_from_this(), ActiveWindow, aButton);

			if(result)
			{
				return (result > 0) ? false : true;
			}
		}
	}

	return Windows[ActiveWindow]->Input(aButton);
}

bool										(*Summerface::BackgroundCallback)					() = 0;

