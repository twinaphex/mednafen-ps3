#include <es_system.h>

						WiiVideo::WiiVideo				()
{
	Screen = SDL_SetVideoMode(640, 480, 32, SDL_FULLSCREEN);
	if(!Screen)
	{
		printf("SDL Couldn't set video mode: %s\n", SDL_GetError());
		throw "SDL couldn't set video mode";
	}
	
	esScreenWidth = 640;
	esScreenHeight = 480;
}

						WiiVideo::~WiiVideo				()
{
}

void					WiiVideo::SetClip				(Area aClip)
{
	ESVideo::SetClip(aClip);
	Area clap = GetClip();
}

void					SetExit							();
void					WiiVideo::Flip					()
{
	SDL_Flip(Screen);

//TODO: Maybe a better place to handle the event loop	
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT)
		{
			SetExit();
		}
	}
	
	if(es_input->ButtonDown(0, 0x80000000 | SDLK_F10))
	{
		SetExit();
	}
//TODO: Done with event loop
	
	SetClip(Area(0, 0, GetScreenWidth(), GetScreenHeight()));

	SDL_FillRect(Screen, 0, 0xFFFFFFFF);
}

void					WiiVideo::PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor, Area* aArea)
{
	aX += esClip.X;
	aY += esClip.Y;

	SDL_Rect texArea, outArea = {aX, aY, aWidth, aHeight};

	if(aArea)
	{
		texArea.x = aArea->X;
		texArea.y = aArea->Y;
		texArea.w = aArea->Width;
		texArea.h = aArea->Height;
	}
	else
	{
		texArea.x = 0;
		texArea.y = 0;
		texArea.w = 640;
		texArea.h = 480;
	}

	SDL_BlitSurface(((WiiTexture*)aTexture)->Surface, &texArea, Screen, &outArea);
}

void					WiiVideo::FillRectangle			(Area aArea, uint32_t aColor)
{
	SDL_Rect t;
	t.x = aArea.X;
	t.y = aArea.Y;
	t.w = aArea.Width;
	t.h = aArea.Height;
	SDL_FillRect(Screen, &t, aColor);
}

void					WiiVideo::PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
{
	Area output = CalculatePresentArea(aAspectOverride, aUnderscan, aUnderscanFine);
}
