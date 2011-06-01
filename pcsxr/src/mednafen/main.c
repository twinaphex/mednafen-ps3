#include "psxcommon.h"
#include "plugins.h"
#include "video_plugin/globals.h"
#include "input_plugin/pad.h"

//System functions, needed by libpcsxcore
extern int wanna_leave;
void				SysUpdate		()		{wanna_leave = 1;}
void				SysRunGui		()		{/* Nothing */}
void				SysReset		()		{EmuReset();}

void				SysClose		()
{
	EmuShutdown();
	PSXPLUG_Close();
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
    strcpy(Config.Bios, BIOSPath);
	strncpy(Config.Mcd1, MCD1, MAXPATHLEN);
	strncpy(Config.Mcd2, MCD2, MAXPATHLEN);

	EmuInit();

	PSXPLUG_Load();

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
	PSXCPU_Execute();

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
				uint16_t* srcpix = &g_gpu.psx_vram.u16[(1024 * (i + y)) + x];
				uint32_t* destpix = &aPixels[aPitch * i];
				for(int j = 0; j != w / 2; j ++)
				{
					uint16_t a = *srcpix++;
					uint16_t b = *srcpix++;
					uint16_t c = *srcpix++;

#ifndef WORDS_BIGENDIAN
					*destpix++ = ((a & 0xFF) << 16) | ((a & 0xFF00)) | ((b & 0xFF));
					*destpix++ = ((b & 0xFF00) << 8) | ((c & 0xFF00) >> 8) | ((c & 0xFF) << 8);
#else
					//Upper 8 bits are ignored on present in mednafen-ps3, may wan't to mask them otherwise...
					*destpix ++ = (a << 8) | (b >> 8);
					*destpix ++ = (b << 16) | c;
#endif
				}
			}
		}
		else
		{
			//TODO: Alitvec could be useful here
			for(int i = 0; i != h; i++)
			{
				uint16_t* srcpix = &g_gpu.psx_vram.u16[(1024 * (i + y)) + x];
				uint32_t* destpix = &aPixels[aPitch * i];
				for(int j = 0; j != w; j++)
				{
					uint16_t s = GETLE16(srcpix++);
					destpix[j] = (((s << 19) & 0xF80000) | ((s << 6) & 0xF800) | ((s >> 7) & 0xF8));
				}
			}
		}
	}
	else if(g_gpu.dsp.mode.x)
	{
		x = g_gpu.dsp.position.x;
	    y = g_gpu.dsp.position.y;
	    w = (g_gpu.dsp.range.x1 - g_gpu.dsp.range.x0) / g_gpu.dsp.mode.x;
	    h = (g_gpu.dsp.range.y1 - g_gpu.dsp.range.y0) * g_gpu.dsp.mode.y;
	}

	*aWidth = w;
	*aHeight = h;

	//Grab the audio
	memcpy(aSound, SoundBuf, SoundBufLen);
	*aSoundLen = SoundBufLen / 4;
	SoundBufLen = 0;
}

