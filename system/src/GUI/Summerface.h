#pragma once

///Class defining a GUI view.
class													Summerface : public Menu, public smartptr::enable_shared_from_this<Summerface>
{
	typedef std::set<SummerfaceInputConduit_Ptr>		ConduitSet;						///<Define a set of SummerfaceInputConduit objects.

	public: //Do not call!
		///Empty constructor.
														Summerface						() {};

	public:
		///Create a new Summerface with no windows.
		///@return Pointer to the new interface.
		static Summerface_Ptr							Create							(); //External

		///Create a new Summerface with a default window.
		///@param aName Name of the window.
		///@param aWindow Pointer to the window.
		///@return Pointer to the new interface.
		///@exception ESException If aWindow is not a valid window.
		static Summerface_Ptr							Create							(const std::string& aName, SummerfaceWindow_Ptr aWindow); //External

		///Empty virtual destructor for Summerface.
		virtual											~Summerface						() {};

		///Draw the background and all windows.
		///@return True to stop interface processing.
		virtual bool									Draw							();

		///Pass input to all input handlers and the active window.
		///@param aButton System button ID of pressed button.
		///@return True to stop interface processing.
		virtual bool									Input							(uint32_t aButton);

		///Add a new SummerfaceWindow to the interface.
		///@param aName Name of the window. It is an error if a window matching aName is already present.
		///@param aWindow Pointer to the window. Must be a valid pointer to a SummerfaceWindow object.
		void											AddWindow						(const std::string& aName, SummerfaceWindow_Ptr aWindow)
		{
			assert(Windows.find(aName) == Windows.end());
			assert(aWindow);
			Windows[aName] = aWindow; ActiveWindow = aName;
		}

		///Remove a SummerfaceWindow from the interface.
		///@param aName Name of the window. It is an error if a matching window is not found.
		void											RemoveWindow					(const std::string& aName)
		{
			assert(Windows.find(aName) != Windows.end());
			Windows.erase(aName);
		}

		///Get a SummerfaceWindow from the interface.
		///@param aName Name of the window. It is an error if a matching window is not found.
		///@return A pointer to the SummerfaceWindow.
		SummerfaceWindow_Ptr							GetWindow						(const std::string& aName)
		{
			assert(Windows.find(aName) != Windows.end());
			return Windows[aName];
		}

		///Set the SummerfaceWindow that receives input from the interface.
		///@param aName Name of the window. It is an error if a matching window is not found.
		void											SetActiveWindow					(const std::string& aName)
		{
			assert(Windows.find(aName) != Windows.end());
			ActiveWindow = aName;
		}

		///Attach a new SummerfaceInputConduit to the interfaces input chain.
		///@param aConduit Pointer to a SummerfaceInputConduit. Apply the same conduit more than once has no effect.
		void											AttachConduit					(SummerfaceInputConduit_Ptr aConduit)
		{
			Handlers.insert(aConduit);
		}

		///Remove a SummerfaceInputConduit from the interfaces input chain.
		///@param aConduit Pointer to a SummerfaceInputConduit to remove. There is no effect if the conduit is not present.
		void											DetachConduit					(SummerfaceInputConduit_Ptr aConduit)
		{
			Handlers.erase(aConduit);
		}

		///Static function to set the callback for drawing the backgroun.
		///@param aCallback Function called to draw the background.
		static void										SetDrawBackground				(bool (*aCallback)()) {BackgroundCallback = aCallback;};

	private:
		ConduitSet										Handlers;						///<Objects in the interface's input chain.

		std::map<std::string, SummerfaceWindow_Ptr>		Windows;						///<List of windows in the interface.
		std::string										ActiveWindow;					///<Name of the interface's active window.

		static bool										(*BackgroundCallback)			();///<Callback function for drawing the background.
};

