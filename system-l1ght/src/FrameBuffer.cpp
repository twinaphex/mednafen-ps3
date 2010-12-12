#include <ps3_system.h>

						Texture::Texture					(uint32_t aWidth, uint32_t aHeight)
{
	if(aWidth == 0 || aHeight == 0 || aWidth > 2048 || aHeight > 2048)
	{
		throw std::string("Texture::Texture: Texture size is invalid, only sizes up to 2048x2048 are supported, and dimensions may not be zero.");
	}

	Width = aWidth;
	Height = aHeight;
	
	PS3Video::Allocate(Pixels, Offset, Width * Height * 4, 16);

	Filter = 0;
}
						
						Texture::~Texture					()
{
	//TODO: Support freeing textures!
}

void					Texture::Clear						(uint32_t aColor)
{
	for(int i = 0; i != Width * Height; i ++)
	{
		Pixels[i] = aColor;
	}
}	

uint32_t*				Texture::GetPixels					()
{
	return Pixels;
}
		
void					Texture::SetFilter					(uint32_t aOn)
{
	Filter = aOn == 0 ? 0 : 1;
}
