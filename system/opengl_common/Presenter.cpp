#include <es_system.h>

#ifndef __CELLOS_LV2__ //Don't need these files on PS3
# include <Cg/cg.h>
# include <Cg/cgGL.h>
#endif

#include "Presenter.h"
#include "Shaders.h"

namespace
{
	CGcontext					Context;
	Texture*					Border;
	GLShader*					Presenter;
}

void							CgPresenter::Initialize				()
{
	Context = cgCreateContext();
	Presenter = new GLShader(Context, "", false, 1);
}

void							CgPresenter::Shutdown				()
{
	//TODO
}

void							CgPresenter::SetFilter				(const std::string& aName, uint32_t aPrescale)
{
	delete Presenter;
	Presenter = GLShader::MakeChainFromPreset(Context, aName, aPrescale);
}

void							CgPresenter::AttachBorder			(Texture* aTexture)
{
	Border = aTexture;
}

void							CgPresenter::Present				(GLuint aID, uint32_t aWidth, uint32_t aHeight, const Area& aViewPort, const Area& aOutput)
{
	float xl = (float)aViewPort.X / (float)aWidth;
	float xr = (float)aViewPort.Right() / (float)aWidth;
	float yl = (float)aViewPort.Y / (float)aHeight;
	float yr = (float)aViewPort.Bottom() / (float)aHeight;

	Presenter->SetViewport(xl, xr, yl, yr);
	Presenter->Set(aOutput, aViewPort.Width, aViewPort.Height, aWidth, aHeight);

	GLuint borderTexture = 0;
	if(Border)
	{
		Border->Apply();
		borderTexture = Border ? Border->GetID() : 0;
	}

	Presenter->Present(aID, borderTexture);
}

