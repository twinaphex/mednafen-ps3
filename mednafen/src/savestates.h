#pragma once

DEFINE_PTR_TYPE(StateLabel);

class					StateLabel : public SummerfaceLabel
{
	public:
						StateLabel				(bool aLoad);
		virtual			~StateLabel				() {delete[] Image;};

		void			SetSlot					(uint32_t aSlot);

		bool			Input					();
		bool			PrepareDraw				();

		void			FillScratch				(uint32_t aSlot);

	protected:
		uint32_t*		Image;
		uint32_t		Slot;
		bool			Load;
};

class					StateMenu
{
	public:
						StateMenu				(bool aLoad); //Extern
		virtual			~StateMenu				() {};

		void			Do						() {UI->Do();};

	private:
		StateLabel_Ptr	Label;
		Summerface_Ptr	UI;
};

