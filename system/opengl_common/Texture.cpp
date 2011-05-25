#include <es_system.h>

						GLTexture::GLTexture				(uint32_t aWidth, uint32_t aHeight) :
	Texture(aWidth, aHeight, aWidth),
	Pixels(0),
	ID(0)
{
	ErrorCheck(esWidth != 0 && esHeight != 0 && esWidth <= 2048 && esHeight <= 2048, "Texture::Texture: Texture size is invalid, only sizes up to 2048x2048 are supported, and dimensions may not be zero. [Width: %d, Height: %d]", esWidth, esHeight);

	Pixels = new uint32_t[esWidth * esHeight];
	glGenTextures(1, &ID);
}
						
						GLTexture::~GLTexture				()
{
	glDeleteTextures(1, &ID);
	delete[] Pixels;
}

void					GLTexture::Clear					(uint32_t aColor)
{
	for(int i = 0; i != esWidth * esHeight; i ++)
	{
		Pixels[i] = aColor;
	}

	esValid = false;
}	

uint32_t*				GLTexture::GetPixels				()
{
	esValid = false;
	return Pixels;
}

void					GLTexture::Apply					()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ID);

	if(!esValid)
	{
		esValid = true;
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#ifndef __CELLOS_LV2__
		glTexImage2D(GL_TEXTURE_2D, 0, 4, esWidth, esHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)Pixels);
#else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ARGB_SCE, esWidth, esHeight, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, (void*)Pixels);
#endif
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, esFilter ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, esFilter ? GL_LINEAR : GL_NEAREST);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

