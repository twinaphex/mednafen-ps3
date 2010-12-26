#include <ps3_system.h>

						SDLVideo::SDLVideo				()
{
	const SDL_VideoInfo* dispinfo = SDL_GetVideoInfo();

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

	Screen = SDL_SetVideoMode(dispinfo->current_w, dispinfo->current_h, 32, SDL_OPENGL);
	if(!Screen)
	{
		throw "SDL couldn't set video mode";
	}
	
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("Mednafen PS3", "Mednafen PS3");
	
	Width = dispinfo->current_w;
	Height = dispinfo->current_h;
		
	FillerTexture = new SDLTexture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);
}

						SDLVideo::~SDLVideo				()
{
	delete FillerTexture;
}

void					SDLVideo::SetClip				(Area aClip)
{
	if(aClip.Right() > GetScreenWidth() || aClip.Bottom() > GetScreenHeight())
	{
		printf("Clip out of range: %d, %d, %d, %d\n", aClip.X, aClip.Y, aClip.Width, aClip.Height);
		Clip = Area(0, 0, GetScreenWidth(), GetScreenHeight());
	}
	else
	{
		Clip = aClip;
	}
}

Area					SDLVideo::GetClip				()
{
	return Clip;
}

//HACK:
void				SetExit					();
void					SDLVideo::Flip					()
{
	SDL_GL_SwapBuffers();
	
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT)
		{
			SetExit();
		}
	}
	
	Clip = Area(0, 0, GetScreenWidth(), GetScreenHeight());
	
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
	aX += Clip.X;
	aY += Clip.Y;

	//TODO: Better clipping
	if(aX + aWidth >= Clip.Right() || aY + aHeight >= (Clip.Bottom() + 10))
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
	Area output(0, 0, GetScreenWidth(), GetScreenHeight());

	float xl = (float)aViewPort.X / (float)aTexture->GetWidth();
	float xr = (float)aViewPort.Right() / (float)aTexture->GetWidth();
	float yl = (float)aViewPort.Y / (float)aTexture->GetHeight();
	float yr = (float)aViewPort.Bottom() / (float)aTexture->GetHeight();

	double underPercent = (double)aUnderscan / 100.0;
						
	double widthP = (aAspectOverride ? GetScreenWidth() : GetScreenWidth() - ((double)GetScreenWidth() * .125)) * (underPercent / 2);
	double heightP = GetScreenHeight() * (underPercent / 2);
		
	if(aAspectOverride)
	{
		output = Area(widthP, heightP, GetScreenWidth() - widthP * 2, GetScreenHeight() - heightP * 2);
	}
	else
	{
		uint32_t barSize = ((double)GetScreenWidth()) * .125;
		output = Area(barSize + widthP, heightP, GetScreenWidth() - barSize * 2 - widthP * 2, GetScreenHeight() - heightP * 2);
	}

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
