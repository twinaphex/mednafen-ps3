#include "psxcommon.h"
#include "plugins.h"
#include "video_plugin/globals.h"
#include "input_plugin/pad.h"

//C++ wrapper functions
const char*			GetMemoryCardName		(int aSlot);		//Get the Memory card filename
const char*			GetBiosName				();					//Get the bios filename
uint32_t			GetRecompiler			();					//Get the recompiler setting

//Plugins functions, from plugins.c
int					OpenPlugins		();
void				ClosePlugins	();

//System functions, needed by libpcsxcore
//Setting wanna_leave to 1 causes the cpu emu to return, but this is a patch by me
extern int			wanna_leave;
void				SysUpdate		()		{wanna_leave = 1;}
void				SysRunGui		()		{/* Nothing */}

void				SysReset		()
{
	//Reset the emulator
	EmuReset();
}

void				SysClose		()
{
	//Free all memory allocated by the emulator
	EmuShutdown();

	//Calls the close function for all of the plugins, then calls libpcsxcore's ReleasePlugins to shut them down
	ClosePlugins();
}

//Main functions for running emu
int					SysLoad			()
{
	//Setup the config structure
    memset(&Config, 0, sizeof(Config));
    Config.PsxAuto = 1;
    Config.Cpu = GetRecompiler() ? CPU_DYNAREC : CPU_INTERPRETER;
    strcpy(Config.PluginsDir, "builtin");
    strcpy(Config.Gpu, "builtin");
    strcpy(Config.Spu, "builtin");
    strcpy(Config.Pad1, "builtin");
    strcpy(Config.Pad2, "builtin");
    strcpy(Config.Cdr, "builtin");
    strcpy(Config.Net, "Disabled");
    strncpy(Config.Bios, GetBiosName(), MAXPATHLEN);
	strncpy(Config.Mcd1, GetMemoryCardName(0), MAXPATHLEN);
	strncpy(Config.Mcd2, GetMemoryCardName(1), MAXPATHLEN);

	SetIsoFile("/opt/media/Games/Sony Playstation/Final Fantasy VII - Disc 1.bin");

	//Init psx cpu emulator and memory mapping
	EmuInit();


	//Open and initialize all of the plugins
	OpenPlugins();

	//Load memory cards
	LoadMcds(Config.Mcd1, Config.Mcd2);

	//Get cdrom ID and init PPF support...
	CheckCdrom();

	//Reset the emulated CPU and Memory
	EmuReset();

	//Prepares the game to run, either runs the CPU thru the bios or finds and loads the game EXE if using the emulated bios
	LoadCdrom();
}

//external sound varibles, move em later
extern uint8_t SoundBuf[48000];
extern uint32_t SoundBufLen;

#define RED(x) (x & 0xff)
#define BLUE(x) ((x>>16) & 0xff)
#define GREEN(x) ((x>>8) & 0xff)
#define COLOR(x) (x & 0xffffff)

void		SysFrame			(uint32_t aSkip, uint32_t* aPixels, uint32_t aPitch, uint32_t aInputPort1, uint32_t aInputPort2, uint32_t* aWidth, uint32_t* aHeight, uint32_t* aSound, uint32_t* aSoundLen)
{
	//Send input
	g.PadState[0].JoyKeyStatus = ~aInputPort1;
	g.PadState[0].KeyStatus = ~aInputPort1;
	g.PadState[1].JoyKeyStatus = ~aInputPort2;
	g.PadState[1].KeyStatus = ~aInputPort2;


	//Run frame
	psxCpu->Execute();

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

