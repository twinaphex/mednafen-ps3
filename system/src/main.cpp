#include <es_system.h>
#include "src/utility/Logger.h"
#include "src/utility/Keyboard.h"

Logger*				es_log;
PathBuild*			es_paths = 0;

namespace
{
	volatile bool	want_to_die = false;
	int				es_argc;
	char**			es_argv;
	void			(*ExitFunction)() = 0;
};

void				ESSUB_Init				();
void				ESSUB_Quit				();
bool				ESSUB_WantToDie			();
bool				ESSUB_WantToSleep		();
std::string			ESSUB_GetBaseDirectory	();

#ifndef	HAVE_ESSUB_ERROR
void				ESSUB_Error				(const char* aMessage)
{
	SummerfaceLabel text(Area(10, 10, 80, 20), aMessage);
	text.SetWordWrap(true);
	Summerface("Error", &text, false).Do();
}
#endif

#ifndef HAVE_ESSUB_GETSTRING
std::string			ESSUB_GetString			(const std::string& aHeader, const std::string& aMessage)
{
	Keyboard kb(Area(10, 10, 80, 80), aHeader, aMessage);
	Summerface("Keyboard", &kb, false).Do();
	return kb.GetText();
}
#endif

#ifndef HAVE_ESSUB_CONFIRM
bool				ESSUB_Confirm			(const char* aMessage, bool* aCancel)
{
	//Create the list
	AnchoredListView<SummerfaceItem> list(Area(10, 10, 80, 20));
	list.SetHeader(aMessage);

	//Add the items
	list.AddItem(new SummerfaceItem("Yes", "CheckIMAGE"));
	list.AddItem(new SummerfaceItem("No", "ErrorIMAGE"));

	//Run the interface
	Summerface("Confirm", &list, false).Do();

	//Return the result
	if(aCancel) *aCancel = list.WasCanceled();
	return !list.WasCanceled() && list.GetSelected()->GetText() == "Yes";
}
#endif

#ifndef HAVE_ESSUB_GETNUMBER
bool				ESSUB_GetNumber			(int64_t& aValue, const char* aHeader, uint32_t aDigits, bool aHex)
{
	SummerfaceNumber number(Area(10, 45, 80, 10), aValue, aDigits, aHex);
	number.SetHeader(aHeader);
	Summerface("NUMB", &number, false).Do();

	if(!number.WasCanceled())
	{
		aValue = number.GetValue();
		return true;
	}

	return false;
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

void				InitES					(void (*aExitFunction)(), int argc, char** argv)
{
	ExitFunction = aExitFunction;
	es_argc = argc;
	es_argv = argv;

	ESSUB_Init();

	es_paths = new PathBuild(ESSUB_GetBaseDirectory());
	Colors::LoadColors();

	ESThreads::Initialize();
	ESNetwork::Initialize();
	ESVideo::Initialize();
	ESAudio::Initialize();

	FontManager::InitFonts();
	ImageManager::SetDirectory(es_paths->Build("assets/png/"));

	ESInput::Initialize();

	es_log = new Logger(Area(10, 10, 80, 80));
}

void				QuitES					()
{
	ESInput::Shutdown();

	FontManager::QuitFonts();
	ImageManager::Purge();

	ESAudio::Shutdown();
	ESVideo::Shutdown();
	ESNetwork::Shutdown();
	ESThreads::Shutdown();

	delete es_paths;

	ESSUB_Quit();

	delete es_log;
}

bool				ESHasArgument			(const std::string& aName)
{
	if(es_argc && es_argv)
	{
		for(int i = 0; i != es_argc; i ++)
		{
			if(aName == es_argv[i])
			{
				return true;
			}
		}
	}

	return false;
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

