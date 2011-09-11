#pragma once

class	Summerface;

///Utility class implementing a cancel interface for a widget.
class													SummerfaceCancelable
{
	public:
		///Create a new cancelable object. By default the Canceled value is false.
														SummerfaceCancelable			() {Canceled = false;}

		///Empty virtual destructor, just in case.
		virtual											~SummerfaceCancelable			() {}

		///Get the state of the Canceled flag.
		///@return True if the Canceled flag is set.
		bool											WasCanceled						() {return Canceled;}

		///Set the state of the Canceled flag.
		///@param aCanceled The new state of the Canceled flag.
		void											SetCanceled						(bool aCanceled) {Canceled = aCanceled;}

	protected:
		bool											Canceled;						///<The Canceled flag.
};

///Abstract base class for Summerface widgets.
class													SummerfaceWindow
{
	public:
		///Create a new SummerfaceWindow.
		///@param aRegion The area of the screen, in percent, that the widget covers.
		///@param aBorder If false no border will be drawn around the widget.
														SummerfaceWindow				(const Area& aRegion, bool aBorder = true);

		///Empty virtual destructor for SummerfaceWindow.
		virtual											~SummerfaceWindow				() {};

		///Function responsible for drawing the border of the widget. After drawing the video clipping region should be set
		///to the window's client area and then Draw should be called.
		///@return True to stop processing the interface.
		virtual bool									PrepareDraw						(); //External

		///Virtual function for drawing the client area of a widget. The function should draw with its bounds being the clipping region.
		///@return True to stop processing the interface.
		virtual bool									Draw							() = 0; //Pure virtual

		///Function for handling input for the widget.
		///@param aButton System button that was pressed.
		///@return True to stop processing the interface.
		virtual bool									Input							(uint32_t aButton) {return aButton == ES_BUTTON_CANCEL;}; //External

		///Function called to set the parent Summerface object. This is called by the Summerface object during
		///Summerface::AddWindow.
		///@param aInterface Pointer to the parent Summerface object. It is an error if this is null.
		///@param aName Name of the window on the Summerface object. It is an error if this is empty.
		void											SetInterface					(Summerface* aInterface, const std::string& aName)
		{
			assert(aInterface);
			assert(aName.length() != 0);

			Interface = aInterface;
			Name = aName;
		}

		///Get a pointer to the parent Summerface object. It is an error to call this function when a valid
		///parent is not present.
		///@return A pointer to the parent Summerface object.
		Summerface*										GetInterface					()
		{
			assert(Interface);
			return Interface;
		}

		///Set the header of the window. The header is drawn in a separate area at the top of the window.
		///@param aHeader printf style format string for the header.
		///@param ... printf style va_args for the header.
		virtual void									SetHeader						(const std::string& aHeader, ...); //External

		///Get a copy of the window's header.
		///@return The window's header.
		virtual std::string								GetHeader						() const {return Header;};

	private:
		static const uint32_t							BorderWidth = 4;				///<Width of a window border.
		static const uint32_t							MarginSize = 4;					///<Amount of blank space inside the window border.

		Summerface*										Interface;						///<Parent interface.

		std::string										Name;							///<Name of the window, according to the parent interface.
		std::string										Header;							///<The window's header.
		Area											Region;							///<The area the window occupies, in percent.

		bool											UseBorder;						///<True if the border should be drawn by prepare draw.

		Color											BackgroundColor;				///<Color used to draw the Window's background.
		Color											TextColor;						///<Color used to draw the Windows's header.
		Color											HeaderColor;					///<Color used to draw the Windows's header.
		Color											BorderColor;					///<Color used to draw the Windows's border.
};

