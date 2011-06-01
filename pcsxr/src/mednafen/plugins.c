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

void*			SysLoadSym			(void *lib, const char *sym)
{
	return PluginStub;
}
//TODO: Now that THATS over, here's the rest of the useful code!

int					OpenPlugins		()
{
	//Tell libpcsxcore to load the plugins (Thats that mess up there)
	LoadPlugins();

	//Open all of the plugins and register any callbacks
	pkCDRopen();

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
	pkCDRclose();
	pkSPUclose();
	pkPADclose();
	pkPADclose();
	pkGPUclose();

	//Tell libpcsxcore to unload the plugins
	ReleasePlugins();
}

