#include <es_system.h>

void					ESVideo::Initialize				()
{
	//Init PSGL
	PSGLinitOptions initOpts = {PSGL_INIT_MAX_SPUS | PSGL_INIT_HOST_MEMORY_SIZE, 1, false, 0, 0, 0, 0, 32 * 1024 * 1024};
	psglInit(&initOpts);
	
	Device = psglCreateDeviceAuto(GL_ARGB_SCE, GL_NONE, GL_MULTISAMPLING_NONE_SCE);
	Context = psglCreateContext();
	psglMakeCurrent(Context, Device);
	psglResetCurrentContext();

	//Get Screen Info
	psglGetRenderBufferDimensions(Device, &ScreenWidth, &ScreenHeight);
	WideScreen = psglGetDeviceAspectRatio(Device) > 1.5f;
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());

	//Some settings
	OpenGLHelp::InitializeState();
	glEnable(GL_VSYNC_SCE);

	// Setup vertex buffer
	VertexBuffer = (GLfloat*)memalign(128, VertexBufferCount * VertexSize * sizeof(GLfloat));
	GLShader::ApplyVertexBuffer(VertexBuffer, true);

	//Texture for FillRectangle
	FillerTexture = new Texture(2, 2);
	FillerTexture->Clear(0xFFFFFFFF);

	//Init shaders
	cgRTCgcInit();
	ShaderContext = cgCreateContext();
	Presenter = new GLShader(ShaderContext, "", false, 1);
}

void					ESVideo::Shutdown				()
{
	delete FillerTexture;
	delete Presenter;

	//Destory ShaderContext

	psglDestroyContext(Context);
	psglDestroyDevice(Device);
	
	psglExit();						

	free(VertexBuffer);
}

void					ESVideo::SetScreenSize			(uint32_t aX, uint32_t aY)
{
	ScreenWidth = aX;
	ScreenHeight = aY;
	OpenGLHelp::InitializeState();
	glDisable(GL_SCISSOR_TEST);
}

void					ESVideo::PresentFrame			(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine)
{
	float xl = (float)aViewPort.X / (float)aTexture->GetWidth();
	float xr = (float)aViewPort.Right() / (float)aTexture->GetWidth();
	float yl = (float)aViewPort.Y / (float)aTexture->GetHeight();
	float yr = (float)aViewPort.Bottom() / (float)aTexture->GetHeight();
	Presenter->SetViewport(xl, xr, yl, yr);

	//Enter present state
	OpenGLHelp::EnterPresentState();

	const Area& output = OpenGLHelp::CalculatePresentArea(aAspectOverride, aUnderscan, aUnderscanFine);
	Presenter->Set(output, aViewPort.Width, aViewPort.Height, aTexture->GetWidth(), aTexture->GetHeight());
	aTexture->Apply();

	GLuint borderTexture = 0;
	if(Border)
	{
		Border->Apply();
		borderTexture = Border ? Border->GetID() : 0;
	}

	Presenter->Present(aTexture->GetID(), borderTexture);

	//Exit present state
	OpenGLHelp::ExitPresentState();

	/* Reset vertex buffer */
	GLShader::ApplyVertexBuffer(VertexBuffer, true);
}

PSGLdevice*				ESVideo::Device;
PSGLcontext*			ESVideo::Context;
CGcontext				ESVideo::ShaderContext;

GLfloat*				ESVideo::VertexBuffer;

Texture*				ESVideo::FillerTexture;
GLShader*				ESVideo::Presenter;

uint32_t				ESVideo::ScreenWidth;
uint32_t				ESVideo::ScreenHeight;
bool					ESVideo::WideScreen;
Area					ESVideo::Clip;
Texture*				ESVideo::Border;

