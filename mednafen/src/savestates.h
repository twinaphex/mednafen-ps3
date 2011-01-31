#ifndef SAVESTATES___H
#define SAVESTATES___H

class					StateMenu : public SummerfaceLabel
{
	public:
						StateMenu				(bool aLoad);
		virtual			~StateMenu				();

		void			Do						();

		bool			Input					();
		bool			PrepareDraw				();

		void			FillScratch				(uint32_t aSlot);

	protected:
		Summerface*		UI;
		bool			Load;
		uint32_t		Slot;
		uint32_t*		Image;
};

#endif

