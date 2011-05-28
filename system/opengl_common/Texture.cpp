#include <es_system.h>

#ifdef __CELLOS_LV2__
#define BUFFER_TARGET_ES	GL_TEXTURE_REFERENCE_BUFFER_SCE
#define glTexImageES(w,h)	glTextureReferenceSCE(GL_TEXTURE_2D, 1, (w), (h), 1, GL_ARGB_SCE, (w) * 4, (GLintptr)0);
#else
#define BUFFER_TARGET_ES	GL_PIXEL_UNPACK_BUFFER_ARB
#define glTexImageES(w,h)	glTexImage2D(GL_TEXTURE_2D, 0, 4, (w), (h), 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)0);
#endif

						GLTexture::GLTexture				(uint32_t aWidth, uint32_t aHeight) :
	Texture(aWidth, aHeight, aWidth),
	ID(0),
	BufferID(0),
	MapCount(0),
	Pixels(0)
{
	ErrorCheck(esWidth != 0 && esHeight != 0 && esWidth <= 2048 && esHeight <= 2048, "Texture::Texture: Texture size is invalid, only sizes up to 2048x2048 are supported, and dimensions may not be zero. [Width: %d, Height: %d]", esWidth, esHeight);

	glGenTextures(1, &ID);
	glGenBuffers(1, &BufferID);

	glBindBuffer(BUFFER_TARGET_ES, BufferID);
	glBufferData(BUFFER_TARGET_ES, aWidth * aHeight * 4, 0, GL_STATIC_DRAW);
}
						
						GLTexture::~GLTexture				()
{
	while(MapCount)
	{
		Unmap();
	}

	glDeleteTextures(1, &ID);
	glDeleteBuffers(1, &BufferID);
}

void					GLTexture::Clear					(uint32_t aColor)
{
	uint32_t* Pixels = Map();

	for(int i = 0; i != esWidth * esHeight; i ++)
	{
		Pixels[i] = aColor;
	}

	Unmap();
}	

uint32_t*				GLTexture::Map						()
{
	esValid = false;

	if(MapCount == 0)
	{
		glBindBuffer(BUFFER_TARGET_ES, BufferID);
		Pixels = (uint32_t*)glMapBuffer(BUFFER_TARGET_ES, GL_READ_WRITE);
		glBindBuffer(BUFFER_TARGET_ES, 0);
	}

	MapCount ++;
	return Pixels;
}

void					GLTexture::Unmap					()
{
	if(MapCount)
	{
		MapCount --;

		if(!MapCount)
		{
			glBindBuffer(BUFFER_TARGET_ES, BufferID);
			glUnmapBuffer(BUFFER_TARGET_ES);
			glBindBuffer(BUFFER_TARGET_ES, 0);
			Pixels = 0;
		}
	}
}

void					GLTexture::Apply					()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ID);

	if(!esValid)
	{
		esValid = true;
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glBindBuffer(BUFFER_TARGET_ES, BufferID);
		glTexImageES(esWidth, esHeight);
		glBindBuffer(BUFFER_TARGET_ES, 0);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, esFilter ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, esFilter ? GL_LINEAR : GL_NEAREST);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

