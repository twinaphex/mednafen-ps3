#include <es_system.h>
#include "FrameBuffer.h"

//Use special naming for PS3 to support managing the texture entirely in video memory.
#ifdef __CELLOS_LV2__
# define	glTexImageBufferES(w,h)		glTexImage2D(GL_TEXTURE_2D, 0, GL_ARGB_SCE, (w), (h), 0, GL_ARGB_SCE, GL_UNSIGNED_INT_8_8_8_8_REV, 0);
#else
# define	glTexImageBufferES(w,h)		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, (w), (h), 0, GL_RGB, GL_INT, 0);
#endif

									FrameBuffer::FrameBuffer		(uint32_t aWidth, uint32_t aHeight) :
	ID(0),
	Width(aWidth),
	Height(aHeight),
	Filter(false),
	Valid(false)
{
	//TODO: Add a sanity check
	glGenTextures(1, &ID);

	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImageBufferES(aWidth, aHeight);
}
						
									FrameBuffer::~FrameBuffer		()
{
	glDeleteTextures(1, &ID);
}

void								FrameBuffer::Apply				()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter ? GL_LINEAR : GL_NEAREST);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}


