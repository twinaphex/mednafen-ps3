#include <ps3_system.h>

						SDLTexture::SDLTexture				(uint32_t aWidth, uint32_t aHeight)
{
	if(aWidth == 0 || aHeight == 0 || aWidth > 2048 || aHeight > 2048)
	{
		Abort("Texture::Texture: Texture size is invalid, only sizes up to 2048x2048 are supported, and dimensions may not be zero.");
	}

	Width = aWidth;
	Height = aHeight;

	Surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, aWidth, aHeight, 32, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);

	Filter = 0;
}
						
						SDLTexture::~SDLTexture				()
{
	SDL_FreeSurface(Surface);
}

void					SDLTexture::Clear					(uint32_t aColor)
{
//	for(int i = 0; i != Width * Height; i ++)
//	{
//		Pixels[i] = aColor;
//	}
}	

uint32_t*				SDLTexture::GetPixels				()
{
	return (uint32_t*)Surface->pixels;
}
		
void					SDLTexture::SetFilter				(uint32_t aOn)
{
	Filter = aOn == 0 ? 0 : 1;
}

