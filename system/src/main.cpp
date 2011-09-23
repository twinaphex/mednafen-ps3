#include <es_system.h>
#include "src/utility//TextViewer.h"
#include "src/utility/Keyboard.h"

TextViewer*			es_log;

namespace
{
	volatile bool	want_to_die = false;
	int				es_argc;
	char**			es_argv;
	void			(*ExitFunction)() = 0;
};

void				LibES::Initialize		(void (*aExitFunction)(), int argc, char** argv)
{
	ExitFunction = aExitFunction;
	es_argc = argc;
	es_argv = argv;

	LibESPlatform::Initialize();

	Colors::Initialize();

	bool result;

	ESThreads::Initialize();
	ESNetwork::Initialize();
	ESVideo::Initialize();

	result = ESAudio::Initialize();
	assert(result);

	FontManager::InitFonts();
	ImageManager::SetDirectory(LibES::BuildPath("assets/png/"));

	ESInput::Initialize();

	es_log = new TextViewer(Area(10, 10, 80, 80), "Begin Log:", false);
	es_log->SetHeader("[Global Log]");
}

void				LibES::Shutdown			()
{
	ESInput::Shutdown();

	FontManager::QuitFonts();
	ImageManager::Purge();

	ESAudio::Shutdown();
	ESVideo::Shutdown();
	ESNetwork::Shutdown();
	ESThreads::Shutdown();


	LibESPlatform::Shutdown();

	delete es_log;
}

bool				LibES::HasArgument		(const std::string& aName)
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

volatile bool		LibES::WantToDie		()
{
	want_to_die = LibESPlatform::WantToDie();

	if(want_to_die && ExitFunction)
	{
		ExitFunction();
		exit(1);
	}
	
	return want_to_die;
}

volatile bool		LibES::WantToSleep		()
{
	return LibESPlatform::WantToSleep();
}

void				LibES::Abort			(const std::string& aMessage)
{
	printf("ABORT: %s\n", aMessage.c_str());
	
	if(ExitFunction)
	{
		ExitFunction();
	}
	
	abort();
}

void				LibES::Error			(const std::string& aMessage, const std::string& aHeader)
{
#ifndef	HAVE_ESSUB_ERROR
	SummerfaceLabel text(Area(20, 35, 60, 30), aMessage);
	text.SetWordWrap(true);
	text.SetHeader(!aHeader.empty() ? aHeader : "Message");
	Summerface("Error", &text, false).Do();
#else
	LibES::Error(aMessage, aHeader);
#endif
}


std::string			LibES::GetString		(const std::string& aHeader, const std::string& aMessage)
{
#ifndef HAVE_ESSUB_GETSTRING
	Keyboard kb(Area(10, 10, 80, 80), aHeader, aMessage);
	Summerface("Keyboard", &kb, false).Do();
	return kb.GetText();
#else
	return LibESPlatform::GetString(aHeader, aMessage);
#endif
}

bool				LibES::Confirm			(const std::string& aMessage, bool* aCancel)
{
#ifndef HAVE_ESSUB_CONFIRM
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
#else
	return LibESPlatform::Confirm(aMessage, aCancel);
#endif
}

bool				LibES::GetNumber		(int64_t* aValue, const std::string& aHeader, uint32_t aDigits, bool aHex)
{
	assert(aValue);

#ifndef HAVE_ESSUB_GETNUMBER
	SummerfaceNumber number(Area(10, 45, 80, 10), *aValue, aDigits, aHex);
	number.SetHeader(aHeader);
	Summerface("NUMB", &number, false).Do();

	if(!number.WasCanceled())
	{
		*aValue = number.GetValue();
		return true;
	}

	return false;
#else
	return LibESPlatform::GetNumber(aValue, aHeader, aDigits, aHex);
#endif
}

std::string			LibES::BuildPath		(const std::string& aPath)
{
	return LibESPlatform::BuildPath(aPath);
}

