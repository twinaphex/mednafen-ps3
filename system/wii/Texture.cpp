#include <es_system.h>

						WiiTexture::WiiTexture				(uint32_t aWidth, uint32_t aHeight)
{
	if(aWidth == 0 || aHeight == 0 || aWidth > 2048 || aHeight > 2048)
	{
		Abort("Texture::Texture: Texture size is invalid, only sizes up to 2048x2048 are supported, and dimensions may not be zero.");
	}

	Width = aWidth;
	Height = aHeight;
	
	Surface = SDL_CreateRGBSurface(SDL_SRCALPHA | SDL_HWSURFACE, Width, Height, 32, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
	Filter = 0;
}
						
						WiiTexture::~WiiTexture				()
{
	SDL_FreeSurface(Surface);
}

void					WiiTexture::Clear					(uint32_t aColor)
{
	SDL_FillRect(Surface, 0, aColor);
}	

uint32_t*				WiiTexture::GetPixels				()
{
	return (uint32_t*)Surface->pixels;
}
	
