#include <es_system.h>
#include "FrameBuffer.h"

//Use special naming for PS3 to support managing the texture entirely in video memory.
#ifdef __CELLOS_LV2__
# define	glTexImageBufferES(w,h)		glTexImage2D(GL_TEXTURE_2D, 0, GL_ARGB_SCE, (w), (h), 0, GL_ARGB_SCE, GL_UNSIGNED_INT_8_8_8_8_REV, 0);
#else
# define	glTexImageBufferES(w,h)		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, (w), (h), 0, GL_RGB, GL_INT, 0);
#endif

#if 0
#define glSplat() {uint32_t i = glGetError(); if(i) {printf("%X\n", i); abort();}}
#else
#define glSplat()
#endif

									FrameBuffer::FrameBuffer		(uint32_t aWidth, uint32_t aHeight, bool aDepthBuffer) :
	ID(0),
	DepthID(0),
	Width(aWidth),
	Height(aHeight),
	Filter(false),
	Valid(false)
{
	//TODO: Add a sanity check
	glGenTextures(1, &ID); glSplat();

	glBindTexture(GL_TEXTURE_2D, ID); glSplat();
	glTexImageBufferES(aWidth, aHeight); glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); glSplat();

	if(aDepthBuffer)
	{
		glGenRenderbuffersEXT(1, &DepthID); glSplat();
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, DepthID); glSplat();
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, aWidth, aHeight); glSplat();
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0); glSplat();
	}
}
						
									FrameBuffer::~FrameBuffer		()
{
	glDeleteTextures(1, &ID); glSplat();
}

void								FrameBuffer::Apply				()
{
	glEnable(GL_TEXTURE_2D); glSplat();
	glBindTexture(GL_TEXTURE_2D, ID); glSplat();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter ? GL_LINEAR : GL_NEAREST); glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter ? GL_LINEAR : GL_NEAREST); glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); glSplat();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); glSplat();
}


