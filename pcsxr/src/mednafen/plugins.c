#include "psxcommon.h"
#include "plugins.h"

int				PluginStub				()
{
	/*Nothing*/
	return 0;
}

void*			SysLoadLibrary		(const char *lib)
{
	/*Nothing*/
	return (void*)1;
}

const char*		SysLibError			()
{
	/*Nothing*/
	return 0;
}

void			SysCloseLibrary		(void *lib)
{
	/*Nothing*/
}

//TODO: So ugly, theres more code after this too!
void			pkCDRinit				();
long            pkCDRshutdown           (void);
long            pkCDRopen               (void);
long            pkCDRclose              (void);
long            pkCDRgetTN              (unsigned char * aBuffer);
long            pkCDRgetTD              (unsigned char aTrack, unsigned char* aBuffer);
long            pkCDRreadTrack          (unsigned char* aPosition);
unsigned char*  pkCDRgetBuffer          (void);
unsigned char*  pkCDRgetBufferSub       (void);
long            pkCDRconfigure          (void);
long            pkCDRtest               (void);
void            pkCDRabout              (void);
long            pkCDRplay               (unsigned char* aTime);
long            pkCDRstop               (void);
long            pkCDRsetfilename        (char* aFileName);
long            pkCDRgetStatus          (struct CdrStat* aStatus);
char*           pkCDRgetDriveLetter     (void);
long            pkCDRreadCDDA           (unsigned char aMinutes, unsigned char aSeconds, unsigned char aFrames, unsigned char* aBuffer);
long            pkCDRgetTE              (unsigned char, unsigned char *, unsigned char *, unsigned char *);


void*			SysLoadSym			(void *lib, const char *sym)
{
    if(strcmp(sym, "CDRinit") == 0) return pkCDRinit;
    if(strcmp(sym, "CDRshutdown") == 0) return pkCDRshutdown;
    if(strcmp(sym, "CDRopen") == 0) return pkCDRopen;
    if(strcmp(sym, "CDRclose") == 0) return pkCDRclose;
    if(strcmp(sym, "CDRgetTN") == 0) return pkCDRgetTN;
    if(strcmp(sym, "CDRgetTD") == 0) return pkCDRgetTD;
    if(strcmp(sym, "CDRreadTrack") == 0) return pkCDRreadTrack;
    if(strcmp(sym, "CDRgetBuffer") == 0) return pkCDRgetBuffer;
    if(strcmp(sym, "CDRgetBufferSub") == 0) return pkCDRgetBufferSub;
    if(strcmp(sym, "CDRconfigure") == 0) return pkCDRconfigure;
    if(strcmp(sym, "CDRtest") == 0) return pkCDRtest;
    if(strcmp(sym, "CDRabout") == 0) return pkCDRabout;
    if(strcmp(sym, "CDRplay") == 0) return pkCDRplay;
    if(strcmp(sym, "CDRstop") == 0) return pkCDRstop;
    if(strcmp(sym, "CDRsetfilename") == 0) return pkCDRsetfilename;
    if(strcmp(sym, "CDRgetStatus") == 0) return pkCDRgetStatus;
    if(strcmp(sym, "CDRgetDriveLetter") == 0) return pkCDRgetDriveLetter;
    if(strcmp(sym, "CDRreadCDDA") == 0) return pkCDRreadCDDA;
    if(strcmp(sym, "CDRgetTE") == 0) return pkCDRgetTE;

	return PluginStub;
}
//TODO: Now that THATS over, here's the rest of the useful code!

int					OpenPlugins		()
{
	//Tell libpcsxcore to load the plugins (Thats that mess up there)
	LoadPlugins();

	//Open all of the plugins and register any callbacks
	CDR_open();

	pkGPUopen();
//	pkGPUregisterCallback(GPUbusy);

	pkSPUopen();
	pkSPUregisterCallback(SPUirq);

	pkPADopen(0);

	return 0;
}

void                ClosePlugins    ()
{
	//Close all of the plugins
	CDR_close();
	pkSPUclose();
	pkPADclose();
	pkPADclose();
	pkGPUclose();

	//Tell libpcsxcore to unload the plugins
	ReleasePlugins();
}

