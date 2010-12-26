#include <ps3_system.h>

						SDLTexture::SDLTexture				(uint32_t aWidth, uint32_t aHeight)
{
	if(aWidth == 0 || aHeight == 0 || aWidth > 2048 || aHeight > 2048)
	{
		Abort("Texture::Texture: Texture size is invalid, only sizes up to 2048x2048 are supported, and dimensions may not be zero.");
	}

	Width = aWidth;
	Height = aHeight;
	
	Pixels = (uint32_t*)malloc(Width * Height * 4);
	glGenTextures(1, &ID);

	Filter = 0;
	
	Valid = false;
}
						
						SDLTexture::~SDLTexture				()
{
	glDeleteTextures(1, &ID);	
	
	free(Pixels);
}

void					SDLTexture::Clear					(uint32_t aColor)
{
	for(int i = 0; i != Width * Height; i ++)
	{
		Pixels[i] = aColor;
	}
}	

uint32_t*				SDLTexture::GetPixels				()
{
	Valid = false;
	return Pixels;
}
		
void					SDLTexture::SetFilter				(uint32_t aOn)
{
	Filter = aOn == 0 ? 0 : 1;
}

void					SDLTexture::Apply					()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ID);
	
	if(!Valid)
	{
		Valid = true;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter ? GL_LINEAR : GL_NEAREST);	
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, Width, Height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)Pixels);
	}
}
