#pragma once
#include "SummerfaceWindow.h"
#include "Colors.h"

class													Area;

/////////////////
//SummerfaceLabel
/////////////////
///Summerface Widget which displays a text string.
class													SummerfaceLabel : public SummerfaceWindow
{
	public:
		///Create a new SummerfaceLabel.
		///@param aRegion Screen Area in % for the widget.
		///@param aFormat printf style format string for the message.
		///@param ... printf style va_args for the message.
														SummerfaceLabel					(const Area& aRegion, const std::string& aFormat, ...);

		///Delete a SummerfaceLabel, does nothing.
		virtual											~SummerfaceLabel				() {}

		///Draw the content of the widget.
		///@return True to end processing of the interface, false to continue.
		virtual bool									Draw							();

		///Get a copy of the widget's message.
		///@return A copy of the widget's message.
		virtual std::string								GetMessage						() {return Message;}

		///Set the content of the message using printf style arguments.
		///@param aFormat printf style format string.
		///@param ... printf style va_args.
		virtual void									SetMessage						(const std::string& aFormat, ...);

		///Append the content of a string to the end of the message.
		///@param aMessage Content to append.
		virtual void									AppendMessage					(const std::string& aMessage) {SetMessage("%s%s", GetMessage().c_str(), aMessage.c_str());}

		///Set the state of word wrapping in the message.
		///@param aOn New state of the word wrap flag.
		void											SetWordWrap						(bool aOn) {Wrap = aOn;}

	private:
		std::string										Message;						///<The displayed message.
		bool											Wrap;							///<True to wrap the message when drawing.
		Color											TextColor;						///<Color used to draw the text.
};

