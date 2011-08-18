#pragma once

class FileSelect;
class TextViewer;
typedef std::vector<std::string> BookmarkList;

class						TextFileViewer
{
	public:
							TextFileViewer				();
		virtual				~TextFileViewer				();

		int					HandleInput					(Summerface* aInterface, const std::string& aWindow, uint32_t aButton);

		void				Display						();

	private:
		TextViewer*			Viewer;						///<Widget to display the text.
		Summerface*			Interface;					///<Interface used for displaying the text file.
		BookmarkList		Bookmarks;					///<Unused bookmark container.
		FileSelect*			Browser;					///<Browser for selecting files.
		bool				Loaded;						///<Is a text file loaded?
};

