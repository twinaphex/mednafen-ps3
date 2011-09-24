#include <es_system.h>

//Use special naming for PS3 to support managing the texture entirely in video memory.
#ifdef __CELLOS_LV2__
# define	BUFFER_TARGET_ES			GL_TEXTURE_REFERENCE_BUFFER_SCE
# define	glTexImageES(w,h)			glTextureReferenceSCE(GL_TEXTURE_2D, 1, (w), (h), 1, GL_ARGB_SCE, (w) * 4, (GLintptr)0);
# define	glGenFramebuffersES			glGenFramebuffersOES
# define	glDeleteFramebuffersES		glDeleteFramebuffersOES
# define	glBindFramebufferES			glBindFramebufferOES
# define	glFramebufferTexture2DES	glFramebufferTexture2DOES
# define	GL_FRAMEBUFFER_ES			GL_FRAMEBUFFER_OES
#else
# define	BUFFER_TARGET_ES			GL_PIXEL_UNPACK_BUFFER_ARB
# define	glTexImageES(w,h)			glTexImage2D(GL_TEXTURE_2D, 0, 4, (w), (h), 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)0);
# define	glGenFramebuffersES			glGenFramebuffersEXT
# define	glDeleteFramebuffersES		glDeleteFramebuffersEXT
# define	glBindFramebufferES			glBindFramebufferEXT
# define	glFramebufferTexture2DES	glFramebufferTexture2DEXT
# define	GL_FRAMEBUFFER_ES			GL_FRAMEBUFFER_EXT
#endif

#if 0
#define glSplat() {uint32_t i = glGetError(); if(i) {printf("%X\n", i); abort();}}
#else
#define glSplat()
#endif


									Texture::Texture				(uint32_t aWidth, uint32_t aHeight) :
	BufferID(0),
	ID(0),
	MapCount(0),
	Pixels(0),
	Width(aWidth),
	Height(aHeight),
	Pitch(aWidth),
	Filter(false),
	Valid(false)
{
	//TODO: Add a sanity check

	glGenTextures(1, &ID); glSplat();
	glGenBuffers(1, &BufferID); glSplat();

	glBindBuffer(BUFFER_TARGET_ES, BufferID); glSplat();
	glBufferData(BUFFER_TARGET_ES, aWidth * aHeight * 4, 0, GL_STATIC_DRAW); glSplat();
}
						
									Texture::~Texture				()
{
	while(MapCount)
	{
		Unmap();
	}

	glDeleteTextures(1, &ID); glSplat();
	glDeleteBuffers(1, &BufferID); glSplat();
}

void								Texture::Clear					(uint32_t aColor)
{
	uint32_t* Pixels = Map();

	for(int i = 0; i != Width * Height; i ++)
	{
		Pixels[i] = aColor;
	}

	Unmap();
}	

uint32_t*							Texture::Map					()
{
	Valid = false;

	if(MapCount == 0)
	{
		glBindBuffer(BUFFER_TARGET_ES, BufferID); glSplat();
		Pixels = (uint32_t*)glMapBuffer(BUFFER_TARGET_ES, GL_READ_WRITE); glSplat();
		glBindBuffer(BUFFER_TARGET_ES, 0); glSplat();
	}

	MapCount ++;
	return Pixels;
}

void								Texture::Unmap					()
{
	if(MapCount)
	{
		MapCount --;

		if(!MapCount)
		{
			glBindBuffer(BUFFER_TARGET_ES, BufferID); glSplat();
			glUnmapBuffer(BUFFER_TARGET_ES); glSplat();
			glBindBuffer(BUFFER_TARGET_ES, 0); glSplat();
			Pixels = 0;
		}
	}
}

void								Texture::Apply					()
{
	glEnable(GL_TEXTURE_2D); glSplat();
	glBindTexture(GL_TEXTURE_2D, ID); glSplat();

	if(!Valid)
	{
		Valid = true;
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); glSplat();

		glBindBuffer(BUFFER_TARGET_ES, BufferID); glSplat();
		glTexImageES(Width, Height); glSplat();
		glBindBuffer(BUFFER_TARGET_ES, 0); glSplat();
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter ? GL_LINEAR : GL_NEAREST); glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter ? GL_LINEAR : GL_NEAREST);	glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); glSplat();
}


