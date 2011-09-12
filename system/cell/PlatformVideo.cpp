#include <es_system.h>
#include <opengl_common/Presenter.h>

#include <sysutil/sysutil_sysparam.h>

namespace
{
	PSGLdevice*								Device;
	PSGLcontext*							Context;
	bool									VSyncOn;
	uint32_t								VideoMode;
	uint32_t								DefaultWidth;
	uint32_t								DefaultHeight;
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
	psglGetDeviceDimensions(Device, &DefaultWidth, &DefaultHeight);
//	WideScreen = psglGetDeviceAspectRatio(Device) > 1.5f;

	//Some settings
	glEnable(GL_VSYNC_SCE);
	VSyncOn = true;

	//Init shaders
	cgRTCgcInit();
	CgPresenter::Initialize();
}

void										ESVideoPlatform::Shutdown				()
{
	CgPresenter::Shutdown();

	psglDestroyContext(Context);
	psglDestroyDevice(Device);
	
	psglExit();
}

void										ESVideoPlatform::Flip					()
{
	psglSwap();
}

//PRESENTER
bool										ESVideoPlatform::SupportsShaders		()
{
	return true;
}

void										ESVideoPlatform::AttachBorder			(Texture* aTexture)
{
	CgPresenter::AttachBorder(aTexture);
}

void										ESVideoPlatform::SetFilter				(const std::string& aName, uint32_t aPrescale)
{
	CgPresenter::SetFilter(aName, aPrescale);
}

void										ESVideoPlatform::Present				(GLuint aID, uint32_t aWidth, uint32_t aHeight, const Area& aViewPort, const Area& aOutput)
{
	CgPresenter::Present(aID, aWidth, aHeight, aViewPort, aOutput);
}


//OTHER
bool										ESVideoPlatform::SupportsVSyncSelect	()
{
	return true;
}

bool										ESVideoPlatform::SupportsModeSwitch		()
{
	return true;
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

void										ESVideoPlatform::SetMode				(uint32_t aIndex)
{
	if(VideoMode != aIndex)
	{
		//Kill old device
		psglMakeCurrent(0, 0);
		psglDestroyDevice(Device);

		//Create new device
		static const uint32_t width[] = {0, 1920, 1280, 0, 720, 720};
		static const uint32_t height[] = {0, 1080, 720, 0, 480, 576};

		PSGLdeviceParameters params;
		memset(&params, 0, sizeof(params));

		params.enable = PSGL_DEVICE_PARAMETERS_DEPTH_FORMAT | PSGL_DEVICE_PARAMETERS_WIDTH_HEIGHT;
		params.depthFormat = GL_NONE;
		params.width = (aIndex == 0) ? DefaultWidth : width[aIndex];
		params.height = (aIndex == 0) ? DefaultHeight : height[aIndex];

		Device = psglCreateDeviceExtended(&params);

		psglMakeCurrent(Context, Device);

		//Get parameters
		uint32_t renderWidth, renderHeight;
		psglGetRenderBufferDimensions(Device, &renderWidth, &renderHeight);
		ESVideo::SetScreenSize(renderWidth, renderHeight);
	}

	VideoMode = aIndex;
}

const ESVideoPlatform::ModeList&			ESVideoPlatform::GetModes				()
{
	static ModeList modes;

	if(modes.size() == 0)
	{
		static struct
		{
			const char* Name;
			uint32_t ID;
		}	modeList[] = 
		{
			{"1080", CELL_VIDEO_OUT_RESOLUTION_1080},
			{"720", CELL_VIDEO_OUT_RESOLUTION_720},
			{"576", CELL_VIDEO_OUT_RESOLUTION_576},
			{"480", CELL_VIDEO_OUT_RESOLUTION_480} 
		};

		for(int i = 0; i != 4; i ++)
		{
			if(cellVideoOutGetResolutionAvailability(CELL_VIDEO_OUT_PRIMARY, modeList[i].ID, 0, 0))
			{
				Mode thisMode;
				thisMode.Name = modeList[i].Name;
				thisMode.ID = modeList[i].ID;
				modes.push_back(thisMode);
			}
		}
	}

	return modes;
}

