#include <es_system.h>

						WiiTexture::WiiTexture				(uint32_t aWidth, uint32_t aHeight, bool aStatic)
{
	if(aWidth == 0 || aHeight == 0 || aWidth > 2048 || aHeight > 2048)
	{
		Abort("Texture::Texture: Texture size is invalid, only sizes up to 2048x2048 are supported, and dimensions may not be zero.");
	}

	Static = aStatic;
	Locked = false;

	Width = aWidth;
	Height = aHeight;
	Pitch = Width + ((Width % 4) ? (4 - Width % 4) : 0);
	AdjustedHeight = Height + ((Height % 4) ? (4 - Height % 4) : 0);
	Filter = 0;

	Pixels = (uint32_t*)memalign(32, Pitch * AdjustedHeight * 4);
	Valid = false;
	

}
						
						WiiTexture::~WiiTexture				()
{
	free(Pixels);
}

void					WiiTexture::Clear					(uint32_t aColor)
{
	for(int i = 0; i != Pitch * AdjustedHeight; i ++)
	{
		Pixels[i] = aColor;
	}

	Valid = false;
}	

uint32_t*				WiiTexture::GetPixels				()
{
//	ErrorCheck(!Static || !Locked, "Texture::GetPixels: Attempt to get data of a locked static texture");

	Valid = false;
	return Pixels;
}
	
#include "Metaphrasis.h"

void					WiiTexture::Apply					()
{
	//Lock static texture
	Locked = true;

	//Convert static textures
	if(Static && !Valid)
	{
		Valid = true;

		uint32_t* newbuf = Metaphrasis::convertBufferToRGBA8(Pixels, Pitch, AdjustedHeight);
		free(Pixels);
		Pixels = newbuf;

		DCFlushRange(Pixels, Pitch * AdjustedHeight * 4);

		//Needed?
		GX_InvalidateTexAll();
	}
	else if(!Valid)
	{
		Valid = true;

		//TODO: Convert texture
		DCFlushRange(Pixels, Width * Height * 4);

		//Needed?
		GX_InvalidateTexAll();

	}

	GX_InitTexObj(&TextureObject, Pixels, Pitch, AdjustedHeight, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_LoadTexObj(&TextureObject, GX_TEXMAP0);
}
