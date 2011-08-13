#include <es_system.h>
#include "textviewer.h"

#include "src/utility/TextViewer.h"
#include "src/utility/Files/FileSelect.h"


					TextFileViewer::TextFileViewer				() :
	Viewer(smartptr::make_shared<TextViewer>(Area(10, 10, 80, 80), "NO FILE", false)),
	Interface(Summerface::Create("TextViewer", Viewer)),
	Browser(smartptr::shared_ptr<FileSelect>(new FileSelect("Select Text File", Bookmarks, ""))),
	Loaded(false)
{
	Interface->AttachConduit(smartptr::make_shared<SummerfaceTemplateConduit<TextFileViewer> >(this));
}

void				TextFileViewer::Display						()
{
	if(!Loaded)
	{
		std::string file = Browser->GetFile();

		if(!file.empty())
		{
			Viewer->Reload(file);
			Loaded = true;
		}
		else
		{
			return;
		}
	}

	Interface->Do();
}

int					TextFileViewer::HandleInput					(Summerface_Ptr aInterface, const std::string& aWindow, uint32_t aButton)
{
	if(aButton == ES_BUTTON_TAB)
	{
		std::string file = Browser->GetFile();
		if(!file.empty())
		{
			Viewer->Reload(file);
		}

		return 1;
	}

	return 0;
}

