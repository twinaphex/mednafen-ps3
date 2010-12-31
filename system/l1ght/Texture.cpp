#include <ps3_system.h>

						L1ghtTexture::L1ghtTexture			(uint32_t aWidth, uint32_t aHeight)
{
	if(aWidth == 0 || aHeight == 0 || aWidth > 2048 || aHeight > 2048)
	{
		Abort("Texture::Texture: Texture size is invalid, only sizes up to 2048x2048 are supported, and dimensions may not be zero.");
	}

	Width = aWidth;
	Height = aHeight;
	
	L1ghtVideo::Allocate(Pixels, Offset, Width * Height * 4, 16);

	Filter = 0;
}
						
						L1ghtTexture::~L1ghtTexture			()
{
	//TODO: Support freeing textures!
}

void					L1ghtTexture::Clear					(uint32_t aColor)
{
	for(int i = 0; i != Width * Height; i ++)
	{
		Pixels[i] = aColor;
	}
}	

uint32_t*				L1ghtTexture::GetPixels				()
{
	return Pixels;
}
		
void					L1ghtTexture::SetFilter				(uint32_t aOn)
{
	Filter = aOn == 0 ? 0 : 1;
}
