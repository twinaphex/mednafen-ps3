#include <es_system.h>

Logger_Ptr			es_log;
ESThreads*			es_threads = 0;
PathBuild*			es_paths = 0;

namespace
{
	volatile bool	want_to_die = false;
	void			(*ExitFunction)() = 0;
};

void				ESSUB_Init				();
void				ESSUB_Quit				();
ESVideo*			ESSUB_MakeVideo			();
ESThreads*			ESSUB_MakeThreads		();
bool				ESSUB_WantToDie			();
bool				ESSUB_WantToSleep		();
std::string			ESSUB_GetBaseDirectory	();

#ifndef	HAVE_ESSUB_ERROR
void				ESSUB_Error				(const char* aMessage)
{
	SummerfaceLabel_Ptr text = smartptr::make_shared<SummerfaceLabel>(Area(10, 10, 80, 20), aMessage);
	Summerface::Create("Error", text)->Do();
}
#endif

#ifndef HAVE_ESSUB_GETSTRING
std::string			ESSUB_GetString			(const std::string& aHeader, const std::string& aMessage)
{
	Keyboard_Ptr kb = smartptr::make_shared<Keyboard>(Area(10, 10, 80, 80), aHeader, aMessage);
	Summerface::Create("Keyboard", kb)->Do();
	return kb->GetText();
}
#endif

void				Abort					(const char* aMessage)
{
	printf("ABORT: %s\n", aMessage);
	
	if(ExitFunction)
	{
		ExitFunction();
	}
	
	abort();
}

void				InitES					(void (*aExitFunction)())
{
	ExitFunction = aExitFunction;

	ESSUB_Init();

	es_paths = new PathBuild(ESSUB_GetBaseDirectory());

	es_threads = ESSUB_MakeThreads();
	ESNetwork::Initialize();
	ESVideo::Initialize();
	ESAudio::Initialize();

	FontManager::InitFonts();
	ImageManager::SetDirectory(es_paths->Build("assets/png/"));

	ESInput::Initialize();

	es_log = smartptr::make_shared<Logger>(Area(10, 10, 80, 80));
}

void				QuitES					()
{
	ESInput::Shutdown();

	FontManager::QuitFonts();
	ImageManager::Purge();

	ESAudio::Shutdown();
	ESVideo::Shutdown();
	ESNetwork::Shutdown();
	delete es_threads;

	delete es_paths;

	ESSUB_Quit();
}

volatile bool		WantToDie				()
{
	want_to_die = ESSUB_WantToDie();

	if(want_to_die && ExitFunction)
	{
		ExitFunction();
		exit(1);
	}
	
	return want_to_die;
}

volatile bool		WantToSleep				()
{
	return ESSUB_WantToSleep();
}

