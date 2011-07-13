#pragma once
#include "SummerfaceWindow.h"

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
														SummerfaceLabel					(const Area& aRegion, const std::string& aMessage) :
			SummerfaceWindow(aRegion),
			Wrap(false)
		{
			SetMessage(aMessage);
		}

		///Create a new SummerfaceLabel.
		///@param aRegion Screen Area in % for the widget.
		///@param aFormat printf style format string for the message.
		///@param ... printf style va_args for the message.
														SummerfaceLabel					(const Area& aRegion, const char* aFormat, ...) :
			SummerfaceWindow(aRegion),
			Wrap(false)
		{
			char array[2048];

			va_list args;
			va_start (args, aFormat);
			vsnprintf(array, 2048, aFormat, args);
			va_end(args);

			SetMessage(array);
		}

		///Delete a SummerfaceLabel, does nothing.
		virtual											~SummerfaceLabel				()
		{

		}

		///Draw the content of the widget.
		///@return True to end processing of the interface, false to continue.
		inline virtual bool								Draw							();

		///Get a copy of the widget's message.
		///@return A copy of the widget's message.
		virtual std::string								GetMessage						()
		{
			return Message;
		}

		///Set the content of the message using printf style arguments.
		///@param aFormat printf style format string.
		///@param ... printf style va_args.
		virtual void									SetMessage						(const char* aFormat, ...)
		{
			char array[2048];

			va_list args;
			va_start (args, aFormat);
			vsnprintf(array, 2048, aFormat, args);
			va_end(args);

			Message = array;
		}

		///Set the content of the message by copying a std::string.
		///@param aMessage New content for the message.
		virtual void									SetMessage						(const std::string& aMessage)
		{
			Message = aMessage;
		}

		///Append the content of a string to the end of the message.
		///@param aMessage Content to append.
		virtual void									AppendMessage					(const std::string& aMessage)
		{
			SetMessage("%s%s", GetMessage().c_str(), aMessage.c_str());
		}

		///Set the state of word wrapping in the message.
		///@param aOn New state of the word wrap flag.
		void											SetWordWrap						(bool aOn)
		{
			Wrap = aOn;
		}

	private:
		std::string										Message;						///<The displayed message.
		bool											Wrap;							///<True to wrap the message when drawing.
};


bool													SummerfaceLabel::Draw			()
{
	//If it fits, just print
	if(!Wrap || FontManager::GetBigFont()->MeasureString(GetMessage().c_str()) < ESVideo::GetClip().Width)
	{
		FontManager::GetBigFont()->PutString(GetMessage().c_str(), 2, 2, Colors::Normal, true);	
	}
	else
	{
		//Some state variables
		size_t position = 0;
		const std::string message = GetMessage();
		std::stringstream line;
		int online = 0;

		//Scan the message
		do
		{
			//Find the next word
			size_t wordpos = message.find_first_of(' ', position);
			std::string word = message.substr(position, wordpos - position);

			//If this word would put the line over the size, print and goto the next line
			if(FontManager::GetBigFont()->MeasureString((line.str() + word).c_str()) > ESVideo::GetClip().Width)
			{
				//Handle the case where one word would excede the total length
				if(line.str().empty())
				{
					FontManager::GetBigFont()->PutString(word.c_str(), 2, 2 + FontManager::GetBigFont()->GetHeight() * online, Colors::Normal, true);
					word = "";
				}
				else
				{
					FontManager::GetBigFont()->PutString(line.str().c_str(), 2, 2 + FontManager::GetBigFont()->GetHeight() * online, Colors::Normal, true);
					line.str("");
				}
				online ++;
			}

			//Add the word to the line cache
			if(!word.empty())
			{
				line << word << " ";
			}

			//Get the position of the next word
			position = (wordpos == std::string::npos) ? wordpos : wordpos + 1;
		}	while(position != std::string::npos);

		//Print the last line if needed
		if(!line.str().empty())
		{
			FontManager::GetBigFont()->PutString(line.str().c_str(), 2, 2 + FontManager::GetBigFont()->GetHeight() * online, Colors::Normal, true);				
		}
	}

	return false;
}

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
														SummerfaceImage					(const Area& aRegion, const std::string& aAsset) :
			SummerfaceWindow(aRegion),
			Image(aAsset)
			{

			}

		///Destructor for SummerfaceImage. Does nothing.
		virtual											~SummerfaceImage				() {};

		///Draw the content of the widget.
		///@return True to end processing of the interface, false to continue.
		virtual bool									Draw							()
		{
			if(ImageManager::GetImage(Image))
			{
				Texture* tex = ImageManager::GetImage(Image);

				uint32_t x = 1, y = 1, w = ESVideo::GetClip().Width - 2, h = ESVideo::GetClip().Height - 2;
				Utility::CenterAndScale(x, y, w, h, tex->GetWidth(), tex->GetHeight());

				ESVideo::PlaceTexture(tex, Area(x, y, w, h), Area(0, 0, tex->GetWidth(), tex->GetHeight()), 0xFFFFFFFF);
			}

			return false;
		}

		///Get a copy of the ImageManager asset name of the displayed image.
		///@return The name of the image.
		virtual std::string								GetImage						()
		{
			return Image;
		}

		///Set the ImageManager asset of the displayed image.
		///@param aAsset The name of the image as stored in ImageManager.
		virtual void									SetImage						(const std::string& aAsset)
		{
			Image = aAsset;
		}

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
														SummerfaceNumber				(const Area& aRegion, int64_t aValue, uint32_t aDigits = 10, bool aHex = false) :
			SummerfaceWindow(aRegion),
			SelectedIndex(31),
			Digits(aDigits),
			Hex(aHex)
		{
			SetValue(aValue);
		}

		///Destructor for SummerfaceNumber. Does nothing.
		virtual											~SummerfaceNumber				()
		{
		}

		///Draw the content of the widget.
		///@return True to end processing of the interface, false to continue.
		virtual bool									Draw							()
		{
			for(int i = i; i != Digits; i ++)
			{
				std::string chara = std::string(1, Value[(32 - Digits) + i]);
				FontManager::GetFixedFont()->PutString(chara.c_str(), 2 + i * FontManager::GetFixedFont()->GetWidth(), 2, ((32 - Digits) + i == SelectedIndex) ? Colors::HighLight : Colors::Normal, true);
			}

			return false;
		}

		///Process input of the widget.
		///@return True to end processing of the interface, false to continue.
		virtual bool									Input							(uint32_t aButton)
		{
			SelectedIndex += (aButton == ES_BUTTON_LEFT) ? -1 : 0;
			SelectedIndex += (aButton == ES_BUTTON_RIGHT) ? 1 : 0;
			SelectedIndex = Utility::Clamp(SelectedIndex, 32 - Digits, 31);

			if(aButton == ES_BUTTON_UP) IncPosition(SelectedIndex);
			if(aButton == ES_BUTTON_DOWN) DecPosition(SelectedIndex);

			Canceled = (aButton == ES_BUTTON_CANCEL);
			return aButton == ES_BUTTON_ACCEPT || aButton == ES_BUTTON_CANCEL;
		}

		///Get the value stored in the widget.
		///@return An integer representation of the widget's value.
		int64_t											GetValue						()
		{
			//Strip leading zeroes
			const char* pvalue = Value;
			while(*pvalue != 0 && *pvalue == '0')
			{
				pvalue ++;
			}

			//Get value based on mode
			int64_t value;
			sscanf(pvalue, Hex ? "%llX" : "%lld", (long long int*)&value);

			return value;
		}

		///Set the value stored in the widget.
		///@param aValue The widget's new value.
		void											SetValue						(int64_t aValue)
		{
			//Print base on mode
			snprintf(Value, 33, Hex ? "%032llX" : "%032lld", (long long int)aValue);
		}

	private:
		///Increment the digit stored in a given position of the value.
		///@param aPosition Position of digit to increment.
		void											IncPosition						(uint32_t aPosition)
		{
			if(aPosition >= (32 - Digits) && aPosition < 32)
			{
				Value[aPosition] ++;

				//Going past 9
				if(Value[aPosition] == '9' + 1)
				{
					//Goto proper value
					Value[aPosition] = Hex ? 'A' : '0';
				}

				//Going past F in hex
				if(Hex && Value[aPosition] == 'F' + 1)
				{
					Value[aPosition] = '0';
				}
			}
		}

		///Decrement the digit stored in a given position of the value.
		///@param aPosition Position of digit to decrement.
		void											DecPosition						(uint32_t aPosition)
		{
			if(aPosition >= (32 - Digits) && aPosition < 32)
			{
				Value[aPosition] --;

				//Going below 0
				if(Value[aPosition] == '0' - 1)
				{
					//Goto appropriate value base on mode
					Value[aPosition] = Hex ? 'F' : '9';
				}

				//Going below A in hex
				if(Hex && Value[aPosition] == 'A' - 1)
				{
					Value[aPosition] = '9';
				}
			}
		}

		char											Value[33];						///<The Value stored in the widget.
		int32_t											SelectedIndex;					///<The currently selected digit in the widget.
		uint32_t										Digits;							///<The total number of digits in the widget.
		bool											Hex;							///<True if the input is in hex.
};


