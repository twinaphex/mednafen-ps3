#include <ps3_system.h>

						SDLVideo::SDLVideo				()
{
	Screen = SDL_SetVideoMode(640, 480, 32, 0);
	if(!Screen)
	{
		throw "SDL couldn't set video mode";
	}
	
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("Mednafen PS3", "Mednafen PS3");
	
	esScreenWidth = 640;
	esScreenHeight = 480;
		
	FillerTexture = new SDLTexture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);
}

						SDLVideo::~SDLVideo				()
{
	delete FillerTexture;
}

void					SetExit							();
void					SDLVideo::Flip					()
{
	SDL_Flip(Screen);

	SDL_FillRect(Screen, 0, 0);

//TODO: Maybe a better place to handle the event loop	
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT)
		{
			SetExit();
		}
	}
	
	((SDLInput*)es_input)->Refresh();
//TODO: Done with event loop
	
	esClip = Area(0, 0, GetScreenWidth(), GetScreenHeight());
}

void					SDLVideo::PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor)
{
	aX += esClip.X;
	aY += esClip.Y;

	//TODO: Better clipping
	if(aX + aWidth >= esClip.Right() || aY + aHeight >= (esClip.Bottom() + 10))
	{
		return;
	}

	SDL_Rect dest = {aX, aY, aWidth, aHeight};
	SDL_BlitSurface(((SDLTexture*)aTexture)->Surface, 0, Screen, &dest);
}

void					SDLVideo::FillRectangle			(Area aArea, uint32_t aColor)
{
	PlaceTexture(FillerTexture, aArea.X, aArea.Y, aArea.Width, aArea.Height, aColor);
}

void					SDLVideo::PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, uint32_t aUnderscan)
{
	Area output = CalculatePresentArea(aAspectOverride, aUnderscan);

	SDL_Rect src = {aViewPort.X, aViewPort.Y, aViewPort.Width, aViewPort.Height};
	SDL_Rect dst = {output.X, output.Y, output.Width, output.Height};

    SDL_BlitSurface(((SDLTexture*)aTexture)->Surface, &src, Screen, &dst);
}
