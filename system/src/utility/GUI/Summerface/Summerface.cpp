#include <es_system.h>
#include "Summerface.h"

											Summerface::Summerface								()
{
}

											Summerface::Summerface								(const std::string& aName, SummerfaceWindow* aWindow)
{
	AddWindow(aName, aWindow);
}


											Summerface::~Summerface								()
{
	//Delete all windows
	for(std::map<std::string, SummerfaceWindow*>::iterator iter = Windows.begin(); iter != Windows.end(); iter ++)
	{
		if(iter->second && !iter->second->GetNoDelete())
		{
			delete iter->second;
		}
	}
}

bool										Summerface::Draw									()
{
	uint32_t screenW = es_video->GetScreenWidth();
	uint32_t screenH = es_video->GetScreenHeight();

	es_video->SetClip(Area(0, 0, screenW, screenH));

	if(BackgroundCallback)
	{
		BackgroundCallback();
	}
	else
	{
		es_video->FillRectangle(Area(0, 0, screenW, screenH), Colors::Border);
	}

	for(std::map<std::string, SummerfaceWindow*>::iterator i = Windows.begin(); i != Windows.end(); i ++)
	{
		if(i->second && i->second->PrepareDraw())
		{
			return true;
		}
	}

	return false;
}

bool										Summerface::Input									()
{
	if(Windows[ActiveWindow])
	{
		return Windows[ActiveWindow]->Input();
	}

	return false;
}

void										Summerface::AddWindow								(const std::string& aName, SummerfaceWindow* aWindow)
{
	if(!Windows[aName] && aWindow)
	{
		aWindow->SetInterface(this, aName);
		Windows[aName] = aWindow;
		ActiveWindow = aName;
	}
	else if(!aWindow)
	{
		throw ESException("Window with name is null. [Name: %s]", aName.c_str());		
	}
	else
	{
		throw ESException("Window with name is already present. [Name: %s]", aName.c_str());
	}
}

void										Summerface::RemoveWindow							(const std::string& aName, bool aDelete)
{
	if(Windows[aName])
	{
		if(aDelete)
		{
			delete Windows[aName];
		}

		Windows[aName] = 0;
	}
	else
	{
		throw ESException("Window with name is not present. [Name: %s]", aName.c_str());
	}
}

SummerfaceWindow*							Summerface::GetWindow								(const std::string& aName)
{
	if(Windows[aName])
	{
		return Windows[aName];
	}
	else
	{
		throw ESException("Window with name is not present. [Name: %s]", aName.c_str());
	}
}

void										Summerface::SetActiveWindow							(const std::string& aName)
{
	if(Windows[aName])
	{
		ActiveWindow = aName;
	}
	else
	{
		throw ESException("Window with name is not present. [Name: %s]", aName.c_str());
	}
}

void										Summerface::SetDrawBackground						(void (*aCallback)())
{
	BackgroundCallback = aCallback;
}

void										(*Summerface::BackgroundCallback)					() = 0;

