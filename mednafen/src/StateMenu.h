#ifndef MDFN_STATE_MENU_H
#define MDFN_STATE_MENU_H

class												MednafenStateMenu;

class												MednafenStateItem : public GridItem
{
	public:
													MednafenStateItem				(uint32_t aSlot, MednafenStateMenu* aMenu);
													~MednafenStateItem				();

		bool										Input							();

		void										Draw							(uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected);

		void										SetImage						(uint8_t* aPixels, uint32_t aWidth, uint32_t aHeight);

	protected:
		int											Slot;

		MednafenStateMenu*							Parent;

		uint32_t*									Pixels;
		uint32_t									Width;
		uint32_t									Height;
};

class												MednafenStateMenu : public WinterfaceIconGrid
{
	public:
													MednafenStateMenu				();
		virtual										~MednafenStateMenu				();

		void										SetStateStatus					(StateStatusStruct* aStatus);
		void										SelectMednafenState				(uint32_t aSlot);

	protected:
		uint32_t									CurrentState;
};

#endif
