#pragma once

DEFINE_PTR_TYPE(StateLabel);

///Summerface widget that handles most of the logic behind the SaveMenu.
class					StateLabel : public SummerfaceLabel
{
	public:
		///Create a new StateLabel.
		///@param aLoad True if this is a load-state menu, false otherwise.
						StateLabel				(bool aLoad);

		///Update the selected save state slot. Typically called by StateLabel::Input.
		///@param aSlot Index of the slot. If is an error if it is not between 1 and 9 inclusive.
		void			SetSlot					(uint32_t aSlot);

		///Overrid of SummerfaceWindow::Input.
		///@param aButton System button that was pressed.
		///@return True to stop processing of the interface.
		bool			Input					(uint32_t aButton);

		///Override of SummerfaceWindow::PrepareDraw. Draws the state's image in the background then
		///continues as normal.
		///@return The result of SummerfaceWindow::PrepareDraw().
		bool			PrepareDraw				();

		///Fill the displayed image with the thumbnail from the selected state. Called automatically by
		///SetSlot().
		void			FillScratch				();

	protected:
		uint32_t		Image[1024*1024];		///<Scratch area for a save state's thumbnail.
		uint32_t		Slot;					///<Currently selected slot.
		bool			Load;					///<True if this menu is being used to load a state, false to save.
};

///Simple class to setup and run a save state menu.
class					StateMenu
{
	public:
		///Create a new StateMenu.
		///@param aLoad True if this is a load-state menu, false otherwise.
		//TODO: boost::make_shared goes crashy here with the 4MB image array in StateLabel.
						StateMenu				(bool aLoad) : Label(StateLabel_Ptr(new StateLabel(aLoad))), UI(Summerface::Create("StateLabel", Label)) {}

		///Empty virtual destructor for StateMenu.
		virtual			~StateMenu				() {};

		///Run the save state interface.
		void			Do						() {UI->Do();};

	private:
		StateLabel_Ptr	Label;					///<Custom Summerface Widget that handles most of StateMenu's work.
		Summerface_Ptr	UI;						///<Internally used Summerface object.
};

