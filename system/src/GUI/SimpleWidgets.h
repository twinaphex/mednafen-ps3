#pragma once
#include "SummerfaceWindow.h"

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
		///@param aMessage Text to display in the message.
														SummerfaceLabel					(const Area& aRegion, const std::string& aMessage);

		///Create a new SummerfaceLabel.
		///@param aRegion Screen Area in % for the widget.
		///@param aFormat printf style format string for the message.
		///@param ... printf style va_args for the message.
														SummerfaceLabel					(const Area& aRegion, const char* aFormat, ...);

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
		virtual void									SetMessage						(const char* aFormat, ...);

		///Set the content of the message by copying a std::string.
		///@param aMessage New content for the message.
		virtual void									SetMessage						(const std::string& aMessage) {Message = aMessage;}

		///Append the content of a string to the end of the message.
		///@param aMessage Content to append.
		virtual void									AppendMessage					(const std::string& aMessage) {SetMessage("%s%s", GetMessage().c_str(), aMessage.c_str());}

		///Set the state of word wrapping in the message.
		///@param aOn New state of the word wrap flag.
		void											SetWordWrap						(bool aOn) {Wrap = aOn;}

	private:
		std::string										Message;						///<The displayed message.
		bool											Wrap;							///<True to wrap the message when drawing.
};

/////////////////
//SummerfaceImage
/////////////////
///Summerface Widget which displays an image.
class													SummerfaceImage : public SummerfaceWindow
{
	public:
		///Create a new SummerfaceImage.
		///@param aRegion Screen Area in % for the widget.
		///@param aAsset Name of the ImageManager asset to display.
														SummerfaceImage					(const Area& aRegion, const std::string& aAsset);

		///Destructor for SummerfaceImage. Does nothing.
		virtual											~SummerfaceImage				() {};

		///Draw the content of the widget.
		///@return True to end processing of the interface, false to continue.
		virtual bool									Draw							();

		///Get a copy of the ImageManager asset name of the displayed image.
		///@return The name of the image.
		virtual std::string								GetImage						() {return Image;}

		///Set the ImageManager asset of the displayed image.
		///@param aAsset The name of the image as stored in ImageManager.
		virtual void									SetImage						(const std::string& aAsset) {Image = aAsset;}

	private:
		std::string										Image;							///<The name of the displayed image.
};

//////////////////
//SummerfaceNumber
//////////////////
class													SummerfaceNumber : public SummerfaceWindow, public SummerfaceCancelable
{
	public:
		///Create a new SummerfaceNumber widget.
		///@param aRegion Screen Area in % for the widget.
		///@param aValue Initial value for the widget.
		///@param aDigits Maximum number of digits allowed in the number.
														SummerfaceNumber				(const Area& aRegion, int64_t aValue, uint32_t aDigits = 10, bool aHex = false);

		///Destructor for SummerfaceNumber. Does nothing.
		virtual											~SummerfaceNumber				() {}

		///Draw the content of the widget.
		///@return True to end processing of the interface, false to continue.
		virtual bool									Draw							();

		///Process input of the widget.
		///@return True to end processing of the interface, false to continue.
		virtual bool									Input							(uint32_t aButton);

		///Get the value stored in the widget.
		///@return An integer representation of the widget's value.
		int64_t											GetValue						();

		///Set the value stored in the widget.
		///@param aValue The widget's new value.
		void											SetValue						(int64_t aValue);

	private:
		///Increment the digit stored in a given position of the value.
		///@param aPosition Position of digit to increment.
		void											IncPosition						(uint32_t aPosition);

		///Decrement the digit stored in a given position of the value.
		///@param aPosition Position of digit to decrement.
		void											DecPosition						(uint32_t aPosition);

		char											Value[33];						///<The Value stored in the widget.
		int32_t											SelectedIndex;					///<The currently selected digit in the widget.
		uint32_t										Digits;							///<The total number of digits in the widget.
		bool											Hex;							///<True if the input is in hex.
};

//////////////////
//SummerfaceButton
//////////////////
class													SummerfaceButton : public SummerfaceLabel
{
	public:
		///Create a new SummerfaceButton.
		///@param aRegion Screen Area in % for the widget.
		///@param aButtonName Name of the button being processed.
														SummerfaceButton				(const Area& aRegion, const std::string& aButtonName);

		///Delete a SummerfaceLabel, does nothing.
		virtual											~SummerfaceButton				() {}

		///Process input of the widget.
		///@return True to end processing of the interface, false to continue.
		virtual bool									Input							(uint32_t aButton);

		///Get the ID of the selected button, or 0xFFFFFFFF if none.
		///@return The ID of the selected button.
		virtual uint32_t								GetButton						() {return ButtonID;}

	private:
		uint32_t										LastButton;						///<Internal state value.
		uint32_t										ButtonID;						///<The ID of the pressed button.
};

