#include <es_system.h>

namespace
{
	HWND										Window;
	HDC											DeviceContext;
	HGLRC										RenderContext;
	
	LRESULT CALLBACK							WindowProc								(HWND aWindow, UINT aMessage, WPARAM aWParam, LPARAM aLParam)
	{
		if((aMessage == WM_SYSCOMMAND) && (aWParam == SC_SCREENSAVE))
		{
			return 0;
		}

		if(aMessage == WM_CLOSE)
		{
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(aWindow, aMessage, aWParam, aLParam);
	}
}

void											ESVideoPlatform::Initialize				(uint32_t& aWidth, uint32_t& aHeight)
{
	//TODO: Error checking!

	WNDCLASS windowClass = {CS_OWNDC, WindowProc, 0, 0, GetModuleHandle(0), 0, 0, 0, 0, "ESWindow"};
	RegisterClass(&windowClass);

	//TODO: Make name configurable
	Window = CreateWindow("ESWindow", "mednafen", WS_POPUP | WS_MAXIMIZE | WS_VISIBLE, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, GetModuleHandle(0), 0);
	DeviceContext = GetDC(Window);

	static const uint32_t formatFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DEPTH_DONTCARE;
	static const PIXELFORMATDESCRIPTOR format = {sizeof(PIXELFORMATDESCRIPTOR), 1, formatFlags, PFD_TYPE_RGBA, 24};
	uint32_t formatID = ChoosePixelFormat(DeviceContext, &format);
	SetPixelFormat(DeviceContext, formatID, &format);
	RenderContext = wglCreateContext(DeviceContext);
	wglMakeCurrent(DeviceContext,RenderContext);

	//Get window size
	RECT windowSize;
	GetWindowRect(Window, &windowSize);
	aWidth = windowSize.right - windowSize.left;
	aHeight = windowSize.bottom - windowSize.top;

	glewInit();
}

void											ESVideoPlatform::Shutdown				()
{
}

void											ESVideoPlatform::Flip					()
{
	SwapBuffers(DeviceContext);
}

bool											ESVideoPlatform::SupportsVSyncSelect	()
{
	return false;
}

bool											ESVideoPlatform::SupportsModeSwitch		()
{
	return false;
}

void											ESVideoPlatform::SetVSync				(bool aOn) {assert(false);}
void											ESVideoPlatform::SetMode				(uint32_t aIndex) {assert(false);}
ESVideoPlatform::ModeList::const_iterator		ESVideoPlatform::GetModes				() {assert(false);}

