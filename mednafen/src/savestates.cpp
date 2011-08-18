#include <mednafen_includes.h>
#include "mednafen_help.h"
#include "savestates.h"

//TODO: Put this somewhere
extern StateStatusStruct*	StateStatusInfo;

							StateLabel::StateLabel					(bool aLoad) :
	SummerfaceLabel(Area(10, 85, 25, 6), "Slot 1"),
	Slot(1),
	Load(aLoad)
{
	MDFNI_SelectState(1);

	Slot = StateStatusInfo ? StateStatusInfo->recently_saved : Slot;
	Slot = (Slot < 1 || Slot > 9) ? 1 : Slot;
	SetSlot(Slot);
}

void						StateLabel::SetSlot						(uint32_t aSlot)
{
	assert(aSlot >= 1 && aSlot <= 9);

	MDFNI_SelectState(Slot);
	SetMessage("Slot %d", Slot);
	FillScratch();
}

bool						StateLabel::Input						(uint32_t aButton)
{
	uint32_t oldSlot = Slot;

	//Get any slot chagnes
	Slot += (aButton == ES_BUTTON_RIGHT) ? 1 : 0;
	Slot -= (aButton == ES_BUTTON_LEFT) ? 1 : 0;
	Slot = Utility::Clamp(Slot, 1, 9);

	//Pass any slot changes on
	if(Slot != oldSlot)
	{
		SetSlot(Slot);
	}

	//Do the action
	if(aButton == ES_BUTTON_ACCEPT)
	{
		MednafenEmu::DoCommand(0, 0, Load ? "DoLoadState" : "DoSaveState");
	}

	//Done
	return aButton == ES_BUTTON_CANCEL || aButton == ES_BUTTON_ACCEPT;
}

bool						StateLabel::PrepareDraw					()
{
	//Draw the thumbnail, if appropriate
	if(StateStatusInfo && StateStatusInfo->gfx && StateStatusInfo->w && StateStatusInfo->h)
	{
		MednafenEmu::Blit(Image, StateStatusInfo->w, StateStatusInfo->h, 1024);
	}

	//Pass it on
	return SummerfaceLabel::PrepareDraw();
}

void						StateLabel::FillScratch					()
{
	//You get the idea...
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


