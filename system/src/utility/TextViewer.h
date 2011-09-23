#pragma once

#include "Scroller.h"

///Summerface widget that displays multiple lines of text with support for scrolling.
class							TextViewer : public SummerfaceWindow
{
	public:
		///Create a new TextViewer widget.
		///@param aRegion Area of the screen, in percent, that the widget will occupy.
		///@param aFileName Either the name of a file, or a raw string, to display.
		///@param aFile True if aFileName specifies a file or false if it is a raw string.
								TextViewer					(const Area& aRegion, const char* aFileName, bool aFile = true);

		///Virtual destructor for TextViewer.
		virtual					~TextViewer					() {};

		///Clear the content from the widget.
		void					Clear						();

		///Clear and load new content into the TextViewer widget.
		///@param aFileName Either the name of a file, or a raw string, to display.
		///@param aFile True if aFileName specifies a file or false if it is a raw string.
		void					Reload						(const char* aFileName, bool aFile = true);

		///Apppend a line to the TextViewer using printf style arguments.
		void					AppendLine					(const char* aMessage, ...);

		///Draw the content of the TextViewer widget.
		///@return True to continue processing the interface, false otherwise.
		virtual bool			Draw						();

		///Handle input for the TextViewer widget.
		///@param aButton ES System button to handle, or 0xFFFFFFFF for none.
		///@return True to continue processing the interface, false otherwise.
		virtual bool			Input						(uint32_t aButton);
		
	protected:
		///Fill the widget with the content of a std::istream.
		///@param aStream Stream containing the content to add.
		void					LoadStream					(std::istream* aStream);
	
		std::vector<std::string>Lines;						///<List of lines in the widget.

		Scroller				Top;						///<The index of the first line to be drawn.
		Scroller				Left;						///<The index of the first column to be drawn

		uint32_t				LongestLine;				///<Length of the longest line in the widget.
		uint32_t				LinesDrawn;					///<Total number of lines drawn by the last call to Draw, used for paging.

		Color					TextColor;					///<Color of the text.
};


