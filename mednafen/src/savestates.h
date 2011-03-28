#ifndef SAVESTATES___H
#define SAVESTATES___H

class					StateMenu : public SummerfaceLabel, public boost::enable_shared_from_this<StateMenu>
{
	public:
						StateMenu				(bool aLoad);
		virtual			~StateMenu				();

		void			Do						();

		bool			Input					();
		bool			PrepareDraw				();

		void			FillScratch				(uint32_t aSlot);

	protected:
		Summerface_Ptr	UI;
		bool			Load;
		uint32_t		Slot;
		uint32_t*		Image;
		bool			Added;
};

#endif

