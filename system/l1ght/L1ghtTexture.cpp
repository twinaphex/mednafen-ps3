#include <es_system.h>

						L1ghtTexture::L1ghtTexture			(uint32_t aWidth, uint32_t aHeight) :
	Texture(aWidth, aHeight, aWidth),
	Pixels(0),
	Offset(0)
{
	ErrorCheck(esWidth != 0 && esHeight != 0 && esWidth <= 2048 && esHeight <= 2048, "Texture::Texture: Texture size is invalid, only sizes up to 2048x2048 are supported, and dimensions may not be zero. [Width: %d, Height: %d]", esWidth, esHeight);
	L1ghtVideo::Allocate(Pixels, Offset, esWidth * esHeight * 4, 16);
}
						
						L1ghtTexture::~L1ghtTexture			()
{
	rsxFree(Pixels);
}

void					L1ghtTexture::Clear					(uint32_t aColor)
{
	for(int i = 0; i != esWidth * esHeight; i ++)
	{
		Pixels[i] = aColor;
	}

	esValid = false;
}	

uint32_t*				L1ghtTexture::GetPixels				()
{
	esValid = false;
	return Pixels;
}
