#include "psxcommon.h"
#include "plugins.h"
#include "video_plugin/globals.h"
#include "input_plugin/pad.h"

//Logging functions
void				SysPrintf		(const char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    vprintf (fmt, args);
    va_end (args);
}

void				SysMessage		(const char *fmt, ...)
{
	va_list args;
	va_start (args, fmt);
	vprintf (fmt, args);
	va_end (args);
}

//Plugins functions, from plugin_kludge.c
int					OpenPlugins		();
void				ClosePlugins	();

//System functions, needed by libpcsxcore
//Setting wanna_leave to 1 causes the cpu emu to return, but this is a patch by me
extern int			wanna_leave;
void				SysUpdate		()
{
	wanna_leave = 1;
}

int					SysInit			()
{
	return 1;
}

void				SysRunGui		()
{
	/* Nothing */
}

void				SysReset		()
{
	/* TODO */
}

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
int					SysLoad			()
{
    memset(&Config, 0, sizeof(Config));
    Config.PsxAuto = 1;
    Config.Cpu = CPU_INTERPRETER;
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

	EmuInit();

	OpenPlugins();
	OpenMCDS();

	CheckCdrom();
	EmuReset();
}

//Execute a frame and grab the frame from the Video plugin.
//This won't work without a patch to the psx cpu to return on vblanks
extern uint8_t SoundBuf[48000];
extern uint32_t SoundBufLen;
//#ifdef __LITTLE_ENDIAN__
#define RED(x) (x & 0xff)
#define BLUE(x) ((x>>16) & 0xff)
#define GREEN(x) ((x>>8) & 0xff)
#define COLOR(x) (x & 0xffffff)
//#elif defined __BIG_ENDIAN__
//#define RED(x) ((x>>24) & 0xff)
//#define BLUE(x) ((x>>8) & 0xff)
//#define GREEN(x) ((x>>16) & 0xff)
//#define COLOR(x) SWAP32(x & 0xffffff)
//#endif


void		SysFrame			(uint32_t* aPixels, uint32_t aPitch, uint32_t aKeys, uint32_t* aWidth, uint32_t* aHeight, uint32_t* aSound, uint32_t* aSoundLen)
{
	//Send input
	g.PadState[0].JoyKeyStatus = ~aKeys;
	g.PadState[0].KeyStatus = ~aKeys;

	//Run frame
	psxCpu->Execute();

	//Grab the frame
	int x = 0, y = 0, w = 320, h = 240;

	if (g_gpu.dsp.mode.x)
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
					uint32_t lu = *((uint32_t *)pD);
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
				for(int j = 0; j != w; j++)
				{
					int s = g_gpu.psx_vram.u16[startxy++];
					destpix[j] = (((s << 19) & 0xf80000) | ((s << 6) & 0xf800) | ((s >> 7) & 0xf8)) | 0xff000000;
				}
			}
		}
	}

	*aWidth = w;
	*aHeight = h;

	//Grab the audio
	printf("%d\n", SoundBufLen);
	memcpy(aSound, SoundBuf, SoundBufLen);
	*aSoundLen = SoundBufLen / 4;
	SoundBufLen = 0;
}

