#include <es_system.h>

void					ESVideoPlatform::Initialize				(uint32_t& aWidth, uint32_t& aHeight)
{
	//Init PSGL
	PSGLinitOptions initOpts = {PSGL_INIT_MAX_SPUS | PSGL_INIT_HOST_MEMORY_SIZE, 1, false, 0, 0, 0, 0, 32 * 1024 * 1024};
	psglInit(&initOpts);
	
	Device = psglCreateDeviceAuto(GL_ARGB_SCE, GL_NONE, GL_MULTISAMPLING_NONE_SCE);
	Context = psglCreateContext();
	psglMakeCurrent(Context, Device);
	psglResetCurrentContext();

	//Get Screen Info
	psglGetRenderBufferDimensions(Device, &aWidth, &aHeight);
//	WideScreen = psglGetDeviceAspectRatio(Device) > 1.5f;

	//Some settings
	glEnable(GL_VSYNC_SCE);

	//Init shaders
	cgRTCgcInit();
}

void					ESVideoPlatform::Shutdown				()
{
	//Destory ShaderContext

	psglDestroyContext(Context);
	psglDestroyDevice(Device);
	
	psglExit();
}

void					ESVideoPlatform::Flip					()
{
	psglSwap();
}

PSGLdevice*				ESVideoPlatform::Device;
PSGLcontext*			ESVideoPlatform::Context;
