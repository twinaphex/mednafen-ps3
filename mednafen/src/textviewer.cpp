#include <es_system.h>
#include "textviewer.h"

#include "src/utility/TextViewer.h"
#include "src/utility/Files/FileSelect.h"


					TextFileViewer::TextFileViewer				() :
	Viewer(new TextViewer(Area(10, 10, 80, 80), "NO FILE", false)),
	Interface(new Summerface("TextViewer", Viewer)),
	Browser(new FileSelect("Select Text File", "", "")),
	Loaded(false)
{
	Interface->AttachConduit(new SummerfaceTemplateConduit<TextFileViewer>(this));
}

					TextFileViewer::~TextFileViewer				()
{
	delete Interface;
	delete Browser;
}

void				TextFileViewer::Display						()
{
	if(!Loaded)
	{
		if(!HandleInput(0, "", ES_BUTTON_TAB))
		{
			return;
		}
	}

	Interface->Do();
}

int					TextFileViewer::HandleInput					(Summerface* aInterface, const std::string& aWindow, uint32_t aButton)
{
	if(aButton == ES_BUTTON_TAB)
	{
		std::string file = Browser->GetFile();
		if(!file.empty())
		{
			Viewer->Reload(file.c_str());
		}

		return 1;
	}

	return 0;
}

