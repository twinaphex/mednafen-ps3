#pragma once

DEFINE_PTR_TYPE(FileSelect);
DEFINE_PTR_TYPE(TextViewer);
typedef std::vector<std::string> BookmarkList;

class						TextFileViewer
{
	public:
							TextFileViewer				();
		virtual				~TextFileViewer				() {}

		int					HandleInput					(Summerface_Ptr aInterface, const std::string& aWindow, uint32_t aButton);

		void				Display						();

	private:
		TextViewer_Ptr		Viewer;						///<Widget to display the text.
		Summerface_Ptr		Interface;					///<Interface used for displaying the text file.
		BookmarkList		Bookmarks;					///<Unused bookmark container.
		FileSelect_Ptr		Browser;					///<Browser for selecting files.
		bool				Loaded;						///<Is a text file loaded?
};

