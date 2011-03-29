#include <es_system.h>

int dup(int a){printf("dup() stub!"); return a;}

namespace
{
	bool			want_to_die = false;
};

void				SetExit					(int32_t aChan)
{
	want_to_die = true;
}

void				ESSUB_Init				()
{
	//HACK: Dumb, but it works
	while(!fatInitDefault());

	VIDEO_Init();
	WPAD_Init();
	AUDIO_Init(0);
}

void				ESSUB_Quit				()
{
}

ESVideo*			ESSUB_MakeVideo			()
{
	return new WiiVideo();
}

ESAudio*			ESSUB_MakeAudio			()
{
	return new WiiAudio();
}

ESInput*			ESSUB_MakeInput			()
{
	ESInput* input = new WiiInput();
	WPAD_SetPowerButtonCallback(SetExit);
	return input;
}

ESNetwork*			ESSUB_MakeNetwork		()
{
	return new WiiNetwork();
}

ESThreads*			ESSUB_MakeThreads		()
{
	return new WiiThreads();
}

volatile bool		ESSUB_WantToDie			()
{
	return want_to_die;
}

volatile bool		ESSUB_WantToSleep		()
{
	return false;
}

std::string			ESSUB_GetBaseDirectory	()
{
	//TODO:
	return std::string("usb:/mednafen/");
}

