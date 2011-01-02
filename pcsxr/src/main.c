#include "psxcommon.h"
#include "plugins.h"
#include "dfxvideo/externals.h"
#include "dfinput/pad.h"

void			laGPUinit();
long			laGPUopen(unsigned long * disp, char * CapText, char * CfgFile);
long			laGPUclose();
long			laGPUshutdown();
void			laGPUupdateLace(void); // VSYNC
uint32_t		laGPUreadStatus(void); // READ STATUS
void			laGPUwriteStatus(uint32_t gdata);
void			laGPUreadDataMem(uint32_t * pMem, int iSize);
uint32_t		laGPUreadData(void);
void			laGPUwriteDataMem(uint32_t * pMem, int iSize);
void			laGPUwriteData(uint32_t gdata);
long			laGPUconfigure(void);
long			laGPUdmaChain(uint32_t * baseAddrL, uint32_t addr);
void			laGPUabout(void); // ABOUT
long			laGPUfreeze(uint32_t ulGetFreezeData, GPUFreeze_t * pF);
void			laGPUshowScreenPic(unsigned char * pMem);
void			laGPUvBlank(int val);
void			laGPUkeypressed(int keycode){}
long			laGPUtest();

long			auSPUopen (void);
long			auSPUinit(void);
long			auSPUshutdown(void);
long			auSPUclose(void);
void			auSPUplaySample(unsigned char u);
void			auSPUwriteRegister(unsigned long u, unsigned short y);
unsigned short	auSPUreadRegister(unsigned long u);
void			auSPUwriteDMA(unsigned short u);
unsigned short	auSPUreadDMA(void);
void			auSPUwriteDMAMem(unsigned short *i, int l);
void			auSPUreadDMAMem(unsigned short *i, int l);
void			auSPUplayADPCMchannel(xa_decode_t *n);
void			auSPUregisterCallback(void (*callback)(void));
long			auSPUconfigure(void);
long			auSPUtest(void);
void			auSPUabout(void);
long			auSPUfreeze(uint32_t i, SPUFreeze_t *j);
void			auSPUasync(uint32_t o);
void			auSPUplayCDDAchannel(short * m, int i);

long			inPADinit(long flags);
long			inPADshutdown(void);
long			inPADopen(unsigned long *Disp);
long			inPADclose(void);
long			inPADquery(void);
unsigned char	inPADstartPoll(int pad);
unsigned char	inPADpoll(unsigned char value);
long			inPADreadPort(int num, PadDataS *pad);
long			inPADreadPort1(PadDataS *pad);
long			inPADreadPort2(PadDataS *pad);
long			inPADkeypressed(void);
long			inPADconfigure(void);
void			inPADabout(void);
long			inPADtest(void);

void ClosePlugins()
{
}

int PI()
{
	return 0;
}


void SysPrintf(const char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    vprintf (fmt, args);
    va_end (args);
}

void SysMessage(const char *fmt, ...)
{
	va_list args;
	va_start (args, fmt);
	vprintf (fmt, args);
	va_end (args);
}

//Plugin loader hack
void*		SysLoadLibrary		(const char *lib)		{return (void*)1;}
const char*	SysLibError			()						{return 0;}
void		SysCloseLibrary		(void *lib)				{/*Nothing*/}

void*		SysLoadSym			(void *lib, const char *sym)
{
	if(strcmp(sym, "GPUinit") == 0) return laGPUinit;
    if(strcmp(sym, "GPUshutdown") == 0) return laGPUshutdown;
    if(strcmp(sym, "GPUopen") == 0) return laGPUopen;
    if(strcmp(sym, "GPUclose") == 0) return laGPUclose;
    if(strcmp(sym, "GPUreadData") == 0) return laGPUreadData;
    if(strcmp(sym, "GPUreadDataMem") == 0) return laGPUreadDataMem;
    if(strcmp(sym, "GPUreadStatus") == 0) return laGPUreadStatus;
    if(strcmp(sym, "GPUwriteData") == 0) return laGPUwriteData;
    if(strcmp(sym, "GPUwriteDataMem") == 0) return laGPUwriteDataMem;
    if(strcmp(sym, "GPUwriteStatus") == 0) return laGPUwriteStatus;
    if(strcmp(sym, "GPUdmaChain") == 0) return laGPUdmaChain;
    if(strcmp(sym, "GPUupdateLace") == 0) return laGPUupdateLace;
    if(strcmp(sym, "GPUkeypressed") == 0) return laGPUkeypressed;
    if(strcmp(sym, "GPUfreeze") == 0) return laGPUfreeze;
    if(strcmp(sym, "GPUshowScreenPic") == 0) return laGPUshowScreenPic;
    if(strcmp(sym, "GPUvBlank") == 0) return laGPUvBlank;
    if(strcmp(sym, "GPUconfigure") == 0) return laGPUconfigure;
    if(strcmp(sym, "GPUtest") == 0) return laGPUtest;
    if(strcmp(sym, "GPUabout") == 0) return laGPUabout;

	if(strcmp(sym, "SPUinit") == 0) return auSPUinit;
	if(strcmp(sym, "SPUshutdown") == 0) return auSPUshutdown;
    if(strcmp(sym, "SPUopen") == 0) return auSPUopen;
    if(strcmp(sym, "SPUclose") == 0) return auSPUclose;
    if(strcmp(sym, "SPUconfigure") == 0) return auSPUconfigure;
    if(strcmp(sym, "SPUabout") == 0) return auSPUabout;
    if(strcmp(sym, "SPUtest") == 0) return auSPUtest;
    if(strcmp(sym, "SPUwriteRegister") == 0) return auSPUwriteRegister;
    if(strcmp(sym, "SPUreadRegister") == 0) return auSPUreadRegister;
    if(strcmp(sym, "SPUwriteDMA") == 0) return auSPUwriteDMA;
    if(strcmp(sym, "SPUreadDMA") == 0) return auSPUreadDMA;
    if(strcmp(sym, "SPUwriteDMAMem") == 0) return auSPUwriteDMAMem;
    if(strcmp(sym, "SPUreadDMAMem") == 0) return auSPUreadDMAMem;
    if(strcmp(sym, "SPUplayADPCMchannel") == 0) return auSPUplayADPCMchannel;
    if(strcmp(sym, "SPUfreeze") == 0) return auSPUfreeze;
    if(strcmp(sym, "SPUregisterCallback") == 0) return auSPUregisterCallback;
    if(strcmp(sym, "SPUasync") == 0) return auSPUasync;
    if(strcmp(sym, "SPUplayCDDAchannel") == 0) return auSPUplayCDDAchannel;

    if(strcmp(sym, "PADinit") == 0) return inPADinit;
    if(strcmp(sym, "PADshutdown") == 0) return inPADshutdown;
    if(strcmp(sym, "PADopen") == 0) return inPADopen;
    if(strcmp(sym, "PADclose") == 0) return inPADclose;
    if(strcmp(sym, "PADquery") == 0) return inPADquery;
    if(strcmp(sym, "PADstartPoll") == 0) return inPADstartPoll;
    if(strcmp(sym, "PADpoll") == 0) return inPADpoll;
    if(strcmp(sym, "PADreadPort") == 0) return inPADreadPort;
    if(strcmp(sym, "PADreadPort1") == 0) return inPADreadPort1;
    if(strcmp(sym, "PADreadPort2") == 0) return inPADreadPort2;
    if(strcmp(sym, "PADkeypressed") == 0) return inPADkeypressed;
    if(strcmp(sym, "PADconfigure") == 0) return inPADconfigure;
    if(strcmp(sym, "PADabout") == 0) return inPADabout;
    if(strcmp(sym, "PADtest") == 0) return inPADtest;


	printf("Not Loaded Sym: %s\n", sym);
	return PI;
}

//Setting wanna_leave to 1 causes the cpu emu to return
extern int wanna_leave;
void				SysUpdate()
{
	wanna_leave = 1;
}

void				SysRunGui()		{/* Nothing */}
void				SysReset()		{/* TODO */}

void SysClose()
{
    EmuShutdown();
    ReleasePlugins();
}

int SysInit()
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
    strcpy(Config.BiosDir, "C:\\Users\\Jason\\Desktop\\");
    strcpy(Config.Bios, "SCPH1001.BIN");

    SetIsoFile("C:\\Users\\Jason\\Desktop\\test.bin");

	EmuInit();
	LoadPlugins();

	CDR_open();

	laGPUopen(0, 0, 0);
	GPU_registerCallback(GPUbusy);

	auSPUopen();
	SPU_registerCallback(SPUirq);

	PAD1_open(0);
	PAD2_open(0);

	CheckCdrom();
	EmuReset();
//	LoadCdrom();
}

//Execute a frame and grab the frame from the Video plugin.
//This won't work without a patch to the psx cpu to return on vblanks
extern uint8_t SoundBuf[48000];
extern uint32_t SoundBufLen;

void		SysFrame			(uint32_t* aPixels, uint32_t aPitch, uint32_t aKeys, uint32_t* aWidth, uint32_t* aHeight, uint32_t* aSound, uint32_t* aSoundLen)
{
	g.PadState[0].JoyKeyStatus = ~aKeys;
	g.PadState[0].KeyStatus = ~aKeys;

	psxCpu->Execute();

	for(int i = 0; i != PSXDisplay.DisplayMode.y; i++)
	{
		int startxy = (1024 * (i + PSXDisplay.DisplayPosition.y)) + PSXDisplay.DisplayPosition.x;
		uint32_t* destpix = &aPixels[aPitch * i];
		for(int j = 0; j != PSXDisplay.DisplayMode.x; j++)
		{
			int s = psxVuw[startxy++];
			destpix[j] = (((s << 19) & 0xf80000) | ((s << 6) & 0xf800) | ((s >> 7) & 0xf8)) | 0xff000000;
		}
	}

	*aWidth = PSXDisplay.DisplayMode.x;
	*aHeight = PSXDisplay.DisplayMode.y;

	memcpy(aSound, SoundBuf, SoundBufLen);
	*aSoundLen = SoundBufLen / 4;
	SoundBufLen = 0;
}
