#include <mednafen_includes.h>
#include "savestates.h"

//TODO: Put this somewhere
extern StateStatusStruct*	States[10];

							StateMenu::StateMenu					(bool aLoad) : SummerfaceWindow(Area(0, 0, 0, 0))
{
	Load = aLoad;
	Slot = 0;

	Image = new uint32_t[1024 * 1024];

	//Refresh the images
	MDFNI_SelectState(0);
	FillScratch(0);

	//We delete ourselves, segfault if this isn't here
	SetNoDelete();

	//Create the UI
	UI = new Summerface("StateImage", this);
	UI->AddWindow("StateLabel", new SummerfaceLabel(Area(10, 85, 25, 6), "Slot 1"));
	UI->SetActiveWindow("StateImage");
}

							StateMenu::~StateMenu					()
{
	delete UI;
	delete[] Image;
}

void						StateMenu::Do							()
{
	UI->Do();
}

bool						StateMenu::Input						()
{
	uint32_t oldSlot = Slot;
	Slot += es_input->ButtonDown(0, ES_BUTTON_RIGHT) ? 1 : 0;
	Slot -= es_input->ButtonDown(0, ES_BUTTON_LEFT) ? 1 : 0;
	Slot = Utility::Clamp(Slot, 0, 8);

	if(Slot != oldSlot)
	{
		MDFNI_SelectState(Slot);
		FillScratch(Slot);

		char buffer[32];
		snprintf(buffer, 32, "Slot %d\n", Slot + 1);
		((SummerfaceLabel*)GetInterface()->GetWindow("StateLabel"))->SetMessage(buffer);
	}

	if(es_input->ButtonDown(0, ES_BUTTON_CANCEL))
	{
		return true;
	}

	if(es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
	{
		MednafenEmu::DoCommand(0, 0, Load ? "DoLoadState" : "DoSaveState");
		return true;
	}

	return false;
}

bool						StateMenu::Draw							()
{
	if(States[Slot] && States[Slot]->gfx && States[Slot]->w && States[Slot]->h)
	{
		MednafenEmu::Blit(Image, States[Slot]->w, States[Slot]->h, 1024);
	}

	return false;
}

void						StateMenu::FillScratch					(uint32_t aSlot)
{
	if(States[aSlot] && States[aSlot]->gfx && States[aSlot]->w && States[aSlot]->w < 1024 && States[aSlot]->h < 1024)
	{
		uint8_t* statepix = (uint8_t*)States[aSlot]->gfx;

		for(int k = 0; k != States[aSlot]->h; k ++)
		{
			for(int j = 0; j != States[aSlot]->w; j ++)
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

