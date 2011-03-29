#include <mednafen_includes.h>
#include "savestates.h"

//TODO: Put this somewhere
extern StateStatusStruct*	StateStatusInfo;

							StateLabel::StateLabel					(bool aLoad) :
	SummerfaceLabel(Area(10, 85, 25, 6), "Slot 1"),
	Image(0),
	Slot(1),
	Load(aLoad)
{
	Image = new uint32_t[1024 * 1024];

	MDFNI_SelectState(0);

	Slot = StateStatusInfo ? StateStatusInfo->recently_saved : Slot;
	SetSlot(Slot);
}

void						StateLabel::SetSlot						(uint32_t aSlot)
{
	MDFNI_SelectState(Slot);
	SetMessage("Slot %d", Slot);
	FillScratch(aSlot);
}

bool						StateLabel::Input						()
{
	uint32_t oldSlot = Slot;

	Slot += es_input->ButtonDown(0, ES_BUTTON_RIGHT) ? 1 : 0;
	Slot -= es_input->ButtonDown(0, ES_BUTTON_LEFT) ? 1 : 0;
	Slot = Utility::Clamp(Slot, 1, 9);

	if(Slot != oldSlot)
	{
		SetSlot(Slot);
	}

	if(es_input->ButtonDown(0, ES_BUTTON_CANCEL))
	{
		return true;
	}

	if(es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
	{
		MednafenEmu::DoCommand(0, Summerface_Ptr(), Load ? "DoLoadState" : "DoSaveState");
		return true;
	}

	return false;
}

bool						StateLabel::PrepareDraw					()
{
	if(StateStatusInfo && StateStatusInfo->gfx && StateStatusInfo->w && StateStatusInfo->h)
	{
		MednafenEmu::Blit(Image, StateStatusInfo->w, StateStatusInfo->h, 1024);
	}

	return SummerfaceLabel::PrepareDraw();
}

void						StateLabel::FillScratch					(uint32_t aSlot)
{
	if(StateStatusInfo && StateStatusInfo->gfx && StateStatusInfo->w && StateStatusInfo->w < 1024 && StateStatusInfo->h < 1024)
	{
		uint8_t* statepix = (uint8_t*)StateStatusInfo->gfx;

		for(int k = 0; k != StateStatusInfo->h; k ++)
		{
			for(int j = 0; j != StateStatusInfo->w; j ++)
			{
				uint32_t r = statepix[0];
				uint32_t g = statepix[1];
				uint32_t b = statepix[2];
				Image[k * 1024 + j] = (r << 16) | (g << 8) | (b) | 0xFF000000;
				statepix += 3;
			}
		}
	}
}

							StateMenu::StateMenu					(bool aLoad) :
	Label(boost::make_shared<StateLabel>(aLoad)),
	UI(Summerface::Create("StateLabel", Label))
{

}
