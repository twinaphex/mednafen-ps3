#include <es_system.h>

						SDLTexture::SDLTexture				(uint32_t aWidth, uint32_t aHeight) :
	Texture(aWidth, aHeight, aWidth),
	Pixels(0),
	ID(0)
{
	ErrorCheck(esWidth != 0 && esHeight != 0 && esWidth <= 2048 && esHeight <= 2048, "Texture::Texture: Texture size is invalid, only sizes up to 2048x2048 are supported, and dimensions may not be zero. [Width: %d, Height: %d]", esWidth, esHeight);

	Pixels = new uint32_t[esWidth * esHeight];
	glGenTextures(1, &ID);
}
						
						SDLTexture::~SDLTexture				()
{
	glDeleteTextures(1, &ID);
	delete[] Pixels;
}

void					SDLTexture::Clear					(uint32_t aColor)
{
	for(int i = 0; i != esWidth * esHeight; i ++)
	{
		Pixels[i] = aColor;
	}

	esValid = false;
}	

uint32_t*				SDLTexture::GetPixels				()
{
	esValid = false;
	return Pixels;
}

void					SDLTexture::Apply					()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ID);

	if(!esValid)
	{
		esValid = true;
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, esWidth, esHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)Pixels);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, esFilter ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, esFilter ? GL_LINEAR : GL_NEAREST);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
