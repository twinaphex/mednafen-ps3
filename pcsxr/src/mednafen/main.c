#include "psxcommon.h"
#include "plugins.h"
#include "video_plugin/globals.h"
#include "input_plugin/pad.h"
#include <src/snes/src/lib/libco/libco.h>

static cothread_t	MainThread;
static cothread_t	EmuThread;

//Plugins functions, from plugins.c
int					OpenPlugins		();
void				ClosePlugins	();

//System functions, needed by libpcsxcore
void				SysUpdate		()		{co_switch(MainThread);}
void				SysRunGui		()		{/* Nothing */}
void				SysReset		()		{EmuReset();}

void				SysClose		()
{
	EmuShutdown();
	ReleasePlugins();
}

//Memory cards
uint32_t			DoesFileExist	(const char* aPath); //This is in mednafen.cpp

static char			MCD1[MAXPATHLEN];
static char			MCD2[MAXPATHLEN];
void				SetMCDS			(const char* aOne, const char* aTwo)
{
	MCD1[0] = MCD2[0] = '\0';

	if(aOne)
	{
		strncpy(MCD1, aOne, MAXPATHLEN);
	}

	if(aTwo)
	{
		strncpy(MCD2, aTwo, MAXPATHLEN);
	}
}

void				OpenMCDS		()
{
	if(!DoesFileExist(MCD1))
	{
		CreateMcd(MCD1);
	}

	if(!DoesFileExist(MCD2))
	{
		CreateMcd(MCD2);
	}

	LoadMcds(MCD1, MCD2);
}

//Bios
static char			BIOSPath[MAXPATHLEN];
void				SetBIOS			(const char* aPath)
{
	if(aPath)
	{
		strncpy(BIOSPath, aPath, MAXPATHLEN);
	}
	else
	{
		BIOSPath[0] = '\0';
	}
}

//Main functions for running emu
extern void intExecute();
int					SysLoad			()
{
    memset(&Config, 0, sizeof(Config));
    Config.PsxAuto = 1;
    strcpy(Config.PluginsDir, "builtin");
    strcpy(Config.Gpu, "builtin");
    strcpy(Config.Spu, "builtin");
    strcpy(Config.Pad1, "builtin");
    strcpy(Config.Pad2, "builtin");
    strcpy(Config.Cdr, "builtin");
    strcpy(Config.Net, "Disabled");
    strcpy(Config.Bios, BIOSPath);
	strncpy(Config.Mcd1, MCD1, MAXPATHLEN);
	strncpy(Config.Mcd2, MCD2, MAXPATHLEN);

	MainThread = co_active();

	//TODO: MAKE SURE THIS IS DELETED LATER
	EmuThread = co_create(65536 * sizeof(void*), intExecute);

	EmuInit();

	OpenPlugins();

	OpenMCDS();

	CheckCdrom();
	EmuReset();
	LoadCdrom();
}

//external sound varibles, move em later
extern uint8_t SoundBuf[48000];
extern uint32_t SoundBufLen;

#define RED(x) (x & 0xff)
#define BLUE(x) ((x>>16) & 0xff)
#define GREEN(x) ((x>>8) & 0xff)
#define COLOR(x) (x & 0xffffff)

void		SysFrame			(uint32_t aSkip, uint32_t* aPixels, uint32_t aPitch, uint32_t aKeys, uint32_t* aWidth, uint32_t* aHeight, uint32_t* aSound, uint32_t* aSoundLen)
{
	//Send input
	g.PadState[0].JoyKeyStatus = ~aKeys;
	g.PadState[0].KeyStatus = ~aKeys;

	//Run frame
	co_switch(EmuThread);

	//Grab the frame
	int x = 0, y = 0, w = 320, h = 240;

	if(!aSkip && g_gpu.dsp.mode.x)
	{
		x = g_gpu.dsp.position.x;
	    y = g_gpu.dsp.position.y;
	    w = (g_gpu.dsp.range.x1 - g_gpu.dsp.range.x0) / g_gpu.dsp.mode.x;
	    h = (g_gpu.dsp.range.y1 - g_gpu.dsp.range.y0) * g_gpu.dsp.mode.y;

		if(g_gpu.status_reg & STATUS_RGB24)
		{
			for(int i = 0; i != h; i++)
			{
				int startxy = ((1024) * (i + y)) + x;
				unsigned char* pD = (unsigned char *)&g_gpu.psx_vram.u16[startxy];
				uint32_t* destpix = (uint32_t *)(aPixels + (i * aPitch));
				for(int j = 0; j != w; j++)
				{
					uint32_t lu = SWAP32(*((uint32_t *)pD));
					destpix[j] = 0xff000000 | (RED(lu) << 16) | (GREEN(lu) << 8) | (BLUE(lu));
					pD += 3;
				}
			}
		}
		else
		{
			for(int i = 0; i != h; i++)
			{
				int startxy = (1024 * (i + y)) + x;
				uint32_t* destpix = &aPixels[aPitch * i];
				for(int j = 0; j != w; j++, startxy ++)
				{
					int s = SWAP16(g_gpu.psx_vram.u16[startxy]);
					destpix[j] = (((s << 19) & 0xf80000) | ((s << 6) & 0xf800) | ((s >> 7) & 0xf8)) | 0xff000000;
				}
			}
		}
	}

	*aWidth = w;
	*aHeight = h;

	//Grab the audio
	memcpy(aSound, SoundBuf, SoundBufLen);
	*aSoundLen = SoundBufLen / 4;
	SoundBufLen = 0;
}

