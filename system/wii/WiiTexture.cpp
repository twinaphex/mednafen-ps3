#include <es_system.h>

						WiiTexture::WiiTexture				(uint32_t aWidth, uint32_t aHeight, bool aStatic) : 
	Static(aStatic),
	Locked(false),
	Width(aWidth),
	Height(aHeight),
	Pitch(MultipleOfFour(aWidth)),
	AdjustedHeight(MultipleOfFour(aHeight)),
	Filter(0),
	Pixels(0),
	Valid(false)
{
	//TODO: Max is 1024x1024 on Wii!
	ErrorCheck(Pitch != 0 && AdjustedHeight != 0 && Pitch <= 2048 && AdjustedHeight <= 2048, "Texture::Texture: Texture size is invalid, only sizes up to 2048x2048 are supported, and dimensions may not be zero. [Width: %d, Height: %d]", Pitch, AdjustedHeight);

	Pixels = (uint32_t*)memalign(32, Pitch * AdjustedHeight * 4);
}
						
						WiiTexture::~WiiTexture				()
{
	free(Pixels);
}

void					WiiTexture::Clear					(uint32_t aColor)
{
	ErrorCheck(!Static || !Locked, "Texture::GetPixels: Attempt to clear a locked static texture");

	for(int i = 0; i != Pitch * AdjustedHeight; i ++)
	{
		Pixels[i] = aColor;
	}

	Valid = false;
}	

uint32_t*				WiiTexture::GetPixels				()
{
	ErrorCheck(!Static || !Locked, "Texture::GetPixels: Attempt to get data of a locked static texture");

	Valid = false;
	return Pixels;
}

void					WiiTexture::Apply					(uint32_t aWidth, uint32_t aHeight)
{
	//Lock static texture
	Locked = true;

	//Convert static textures
	if(!Valid)
	{
		Valid = true;

		//Process size
		aWidth = MultipleOfFour(aWidth);
		aHeight = MultipleOfFour(aHeight);

		//Always convert entire static texture
		aWidth = Static ? Pitch : aWidth;
		aHeight = Static ? AdjustedHeight : aHeight;

		//Clamp to real texture size
		aWidth = (aWidth > Pitch) ? Pitch : aWidth;
		aHeight = (aHeight > AdjustedHeight) ? AdjustedHeight : aHeight;

		//Convert texture
		uint16_t bufferCache[Pitch * 4 * 2];

		for(int i = 0; i != aHeight / 4; i ++)
		{
			for(int j = 0; j != aWidth / 4; j ++)
			{
				for(int k = 0; k != 16; k ++)
				{
					int x = (j * 4) + (k % 4);
					int y = (k / 4);
					uint32_t pixel = Pixels[(i * Pitch * 4) + (y * Pitch) + x];

					bufferCache[j * 32 + k] = pixel >> 16;
					bufferCache[j * 32 + k + 16] = pixel & 0xFFFF;
				}
			}

			memcpy(&Pixels[i * Pitch * 4], bufferCache, Pitch * 4 * 4);
		}

		//Flush and load the texture
		DCFlushRange(Pixels, Pitch * AdjustedHeight * 4);
		GX_InvalidateTexAll();
		GX_InitTexObj(&TextureObject, Pixels, Pitch, AdjustedHeight, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	}

	GX_InitTexObjFilterMode(&TextureObject, Filter ? GX_LINEAR : GX_NEAR, Filter ? GX_LINEAR : GX_NEAR);
	GX_LoadTexObj(&TextureObject, GX_TEXMAP0);

	Mtx matrix;
	guMtxIdentity(matrix);
	guMtxScale(matrix, 1.0f / (float)Pitch, 1.0f / (float)AdjustedHeight, 1);
	GX_LoadTexMtxImm(matrix, GX_TEXMTX0, GX_MTX3x4);
}
