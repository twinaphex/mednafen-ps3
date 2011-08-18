#include <es_system.h>
#include "Summerface.h"

											Summerface::Summerface								(const std::string& aName, SummerfaceWindow* aWindow, bool aAssumeOwnership)
{
	if(aWindow)
	{
		AddWindow(aName, aWindow, aAssumeOwnership);
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
	for(WindowSet::iterator i = OwnedWindows.begin(); i != OwnedWindows.end(); i ++)
	{
		delete *i;
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
	for(WindowMap::iterator i = Windows.begin(); i != Windows.end(); i ++)
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

void										Summerface::AddWindow								(const std::string& aName, SummerfaceWindow* aWindow, bool aAssumeOwnership)
{
	assert(Windows.find(aName) == Windows.end());
	assert(aWindow);

	Windows[aName] = aWindow; ActiveWindow = aName;

	if(aAssumeOwnership)
	{
		OwnedWindows.insert(aWindow);
	}
}

///Remove a SummerfaceWindow from the interface.
///@param aName Name of the window. It is an error if a matching window is not found.
void										Summerface::RemoveWindow							(const std::string& aName)
{
	assert(Windows.find(aName) != Windows.end());

	WindowSet::iterator windowIterator = OwnedWindows.find(Windows.find(aName)->second);
	if(windowIterator != OwnedWindows.end())
	{
		delete *windowIterator;
		OwnedWindows.erase(windowIterator);
	}

	Windows.erase(aName);
}


bool										(*Summerface::BackgroundCallback)					() = 0;

