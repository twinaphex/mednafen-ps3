#include <ps3_system.h>

						SDLVideo::SDLVideo				()
{
	const SDL_VideoInfo* dispinfo = SDL_GetVideoInfo();

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

	Screen = SDL_SetVideoMode(dispinfo->current_w, dispinfo->current_h, 32, SDL_OPENGL | SDL_FULLSCREEN);
	if(!Screen)
	{
		printf("SDL Couldn't set video mode: %s\n", SDL_GetError());
		throw "SDL couldn't set video mode";
	}
	
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("Mednafen PS3", "Mednafen PS3");
	
	esScreenWidth = dispinfo->current_w;
	esScreenHeight = dispinfo->current_h;
		
	FillerTexture = new SDLTexture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);
}

						SDLVideo::~SDLVideo				()
{
	delete FillerTexture;
}

void					SetExit							();
void					SDLVideo::Flip					()
{
	SDL_GL_SwapBuffers();

//TODO: Maybe a better place to handle the event loop	
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT)
		{
			SetExit();
		}
	}
	
	((SDLInput*)es_input)->Refresh();

	if(es_input->ButtonDown(0, 0x80000000 | SDLK_F10))
	{
		SetExit();
	}
//TODO: Done with event loop
	
	esClip = Area(0, 0, GetScreenWidth(), GetScreenHeight());
	
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, GetScreenWidth(), GetScreenHeight(), 0, -1, 1);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void					SDLVideo::PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor)
{
	aX += esClip.X;
	aY += esClip.Y;

	//TODO: Better clipping
	if(aX + aWidth >= esClip.Right() || aY + aHeight >= (esClip.Bottom() + 10))
	{
		return;
	}
	
	float r = (float)((aColor >> 24) & 0xFF) / 256.0f;
	float g = (float)((aColor >> 16) & 0xFF) / 256.0f;	
	float b = (float)((aColor >> 8) & 0xFF) / 256.0f;
	float a = (float)((aColor >> 0) & 0xFF) / 256.0f;	

	((SDLTexture*)aTexture)->Apply();
	glBegin(GL_QUADS);
		glColor4f(r,g,b,a);
		glTexCoord2f(0, 0);
		glVertex3f(aX, aY, 0);
		glColor4f(r,g,b,a);
		glTexCoord2f(1, 0);
		glVertex3f(aX + aWidth, aY, 0);
		glColor4f(r,g,b,a);		
		glTexCoord2f(1, 1);		
		glVertex3f(aX + aWidth, aY + aHeight, 0);
		glColor4f(r,g,b,a);		
		glTexCoord2f(0, 1);		
		glVertex3f(aX, aY + aHeight, 0);
	glEnd();
}

void					SDLVideo::FillRectangle			(Area aArea, uint32_t aColor)
{
	PlaceTexture(FillerTexture, aArea.X, aArea.Y, aArea.Width, aArea.Height, aColor);
}

void					SDLVideo::PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, uint32_t aUnderscan)
{
	Area output = CalculatePresentArea(aAspectOverride, aUnderscan);

	float xl = (float)aViewPort.X / (float)aTexture->GetWidth();
	float xr = (float)aViewPort.Right() / (float)aTexture->GetWidth();
	float yl = (float)aViewPort.Y / (float)aTexture->GetHeight();
	float yr = (float)aViewPort.Bottom() / (float)aTexture->GetHeight();

	((SDLTexture*)aTexture)->Apply();
	glDisable(GL_BLEND);
	glBegin(GL_QUADS);
		glColor4f(1,1,1,1);	
		glTexCoord2f(xl, yl);
		glVertex3f(output.X, output.Y, 0);
		glColor4f(1,1,1,1);	
		glTexCoord2f(xr, yl);		
		glVertex3f(output.X + output.Width, output.Y, 0);
		glColor4f(1,1,1,1);	
		glTexCoord2f(xr, yr);
		glVertex3f(output.X + output.Width, output.Y + output.Height, 0);
		glColor4f(1,1,1,1);	
		glTexCoord2f(xl, yr);		
		glVertex3f(output.X, output.Y + output.Height, 0);
	glEnd();
	glEnable(GL_BLEND);
}
