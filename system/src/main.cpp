#include <es_system.h>

Logger*				es_log = 0;
ESVideo*			es_video = 0;
ESAudio*			es_audio = 0;
ESInput*			es_input = 0;
ESNetwork*			es_network = 0;
PathBuild*			es_paths = 0;

namespace
{
	volatile bool	want_to_die = false;
	void			(*ExitFunction)() = 0;
};

void				ESSUB_Init				();
void				ESSUB_Quit				();
ESVideo*			ESSUB_MakeVideo			();
ESAudio*			ESSUB_MakeAudio			();
ESInput*			ESSUB_MakeInput			();
ESNetwork*			ESSUB_MakeNetwork		();
bool				ESSUB_WantToDie			();
bool				ESSUB_WantToSleep		();
std::string			ESSUB_GetBaseDirectory	();

#ifndef	HAVE_ESSUB_ERROR
void				ESSUB_Error				(const char* aMessage)
{
	SummerfaceLabel* text = new SummerfaceLabel(Area(10, 10, 80, 20), aMessage);
	Summerface("Error", text).Do();
}
#endif

#ifndef HAVE_ESSUB_GETSTRING
std::string			ESSUB_GetString			(const std::string& aHeader, const std::string& aMessage)
{
	Keyboard* kb = new Keyboard(Area(10, 10, 80, 80), aHeader, aMessage);
	Summerface sface("Keyboard", kb); sface.Do();
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

	es_video = ESSUB_MakeVideo();
	es_audio = ESSUB_MakeAudio();
	es_input = ESSUB_MakeInput();
	es_network = ESSUB_MakeNetwork();

	FontManager::InitFonts();
	ImageManager::CreateScratch();
	ImageManager::LoadDirectory(es_paths->Build("assets/png/"));

	es_log = new Logger(Area(10, 10, 80, 80));
	es_log->SetNoDelete();
}

void				QuitES					()
{
	delete es_log;

	FontManager::QuitFonts();

	delete es_network;
	delete es_input;
	delete es_audio;
	delete es_video;

	ImageManager::Purge();

	ESSUB_Quit();
	
	delete es_paths;
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
