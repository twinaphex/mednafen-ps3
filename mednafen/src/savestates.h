#ifndef SAVESTATES___H
#define SAVESTATES___H

class					StateMenu : public SummerfaceWindow
{
	public:
						StateMenu				(bool aLoad);
						~StateMenu				();

		void			Do						();

		bool			Input					();
		bool			Draw					();

		void			FillScratch				(uint32_t aSlot);

	protected:
		Summerface*		UI;
		bool			Load;
		uint32_t		Slot;
		uint32_t*		Image;
};

#endif

