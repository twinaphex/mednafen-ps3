#include <es_system.h>
#include "Presenter.h"
#include "ShaderChain.h"

namespace
{
	Texture*					Border;
	GLShader*					PresenterO;
}

void							LibESGL::Presenter::Initialize				()
{
	std::string path = LibES::BuildPath("assets/presets/stock.conf");	
	PresenterO = GLShader::MakeChainFromPreset(path.c_str(), 1);
}

void							LibESGL::Presenter::Shutdown				()
{
	Utility::Delete(PresenterO);
}

void							LibESGL::Presenter::SetFilter				(const std::string& aName, uint32_t aPrescale)
{
	delete PresenterO;
	PresenterO = GLShader::MakeChainFromPreset(aName, aPrescale);
}

void							LibESGL::Presenter::AttachBorder			(Texture* aTexture)
{
	Border = aTexture;
}

void							LibESGL::Presenter::Present					(GLuint aID, uint32_t aWidth, uint32_t aHeight, const Area& aViewPort, const Area& aOutput, bool aFlip)
{
	float xl = (float)aViewPort.X / (float)aWidth;
	float xr = (float)aViewPort.Right() / (float)aWidth;
	float yl = (float)aViewPort.Y / (float)aHeight;
	float yr = (float)aViewPort.Bottom() / (float)aHeight;

	if(aFlip)
	{
		std::swap(yl, yr);
	}

	PresenterO->SetViewport(xl, xr, yl, yr);
	PresenterO->Set(aOutput, aViewPort.Width, aViewPort.Height, aWidth, aHeight);

	GLuint borderTexture = 0;
	if(Border)
	{
		Border->Apply();
		borderTexture = Border ? Border->GetID() : 0;
	}

	PresenterO->Present(aID, borderTexture);
}

