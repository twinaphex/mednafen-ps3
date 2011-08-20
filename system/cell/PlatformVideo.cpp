#include <es_system.h>

namespace
{
	PSGLdevice*								Device;
	PSGLcontext*							Context;
	bool									VSyncOn;
}

void										ESVideoPlatform::Initialize				(uint32_t& aWidth, uint32_t& aHeight)
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
	VSyncOn = true;

	//Init shaders
	cgRTCgcInit();
}

void										ESVideoPlatform::Shutdown				()
{
	//Destory ShaderContext

	psglDestroyContext(Context);
	psglDestroyDevice(Device);
	
	psglExit();
}

void										ESVideoPlatform::Flip					()
{
	psglSwap();
}

bool										ESVideoPlatform::SupportsVSyncSelect	()
{
	return true;
}

bool										ESVideoPlatform::SupportsModeSwitch		()
{
	return false;
}

void										ESVideoPlatform::SetVSync				(bool aOn)
{
	if(aOn != VSyncOn)
	{
		if(aOn)
		{
			glEnable(GL_VSYNC_SCE);
		}
		else
		{
			glDisable(GL_VSYNC_SCE);
		}

		VSyncOn = aOn;
	}
}

void										ESVideoPlatform::SetMode				(uint32_t aIndex) {assert(false);}
ESVideoPlatform::ModeList::const_iterator	ESVideoPlatform::GetModes				() {assert(false);}

