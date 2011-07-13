#pragma once

///Abstract class for running sub-loops for the GUI.
class								Menu
{
	public:
		///Create a new Menu.
									Menu						() : InputWait(true) {}

		///Destroy a Menu object. Has no function but to provide a virtual destructor for sub-classes.
		virtual						~Menu						() {}

		///Set the InputWait flag. If true (the default) the Menu will wait for a system key press before continuing
		///otherwise all input will be handled.
		///@param aInputWait The new state of the InputWait flag.
		void						SetInputWait				(bool aInputWait) {InputWait = aInputWait;}

		///Abstract function for drawing the Menu. The function should use the ESVideo interface to draw screen. The
		///function is not resposible for calling ESVideo::Flip().
		///@return True to stop processing of the Menu.
		virtual bool				Draw						() = 0;

		///Abstract function for handling the input of the Menu.
		///@param aButton Button that was pressed, or 0xFFFFFFFF for none.
		///@return True to stop processing of the Menu.
		virtual bool				Input						(uint32_t aButton) = 0;

		///Run the loop of the Menu.
		void						Do							()
		{
			//Draw the inital screen
			Draw();
			ESVideo::Flip();

			while(!WantToDie())
			{
				//Input
				if(!WantToSleep() && Input(ESInput::WaitForESKey(0, InputWait)))
				{
					break;
				}

				//Drawing
				if(Draw())
				{
					break;
				}
	
				//Redraw the screen
				ESVideo::Flip();
			}

			//Reset input before leaving
			ESInput::Reset();
		}

	private:
		bool					InputWait;						///<True if the Menu should wait for input each iteration.
};

