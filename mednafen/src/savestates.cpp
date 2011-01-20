#include <mednafen_includes.h>
#include "savestates.h"

//TODO: Put this somewhere
extern StateStatusStruct*	States[10];

							StateMenu::StateMenu					(bool aLoad) : SummerfaceWindow(Area(0, 0, 0, 0))
{
	Load = aLoad;
	Slot = 0;

	//Refresh the images
	for(int i = 0; i != 9; i ++)
	{
		MDFNI_SelectState(i);
		FillScratch(i);
	}

	//We delete ourselves, segfault if this isn't here
	SetNoDelete();

	//Create the UI
	UI = new Summerface("StateImage", this);
	UI->AddWindow("StateLabel", new SummerfaceLabel(Area(10, 85, 25, 6), ""));
	UI->SetActiveWindow("StateImage");
}

							StateMenu::~StateMenu					()
{
	delete UI;
}

void						StateMenu::Do							()
{
	UI->Do();
}

bool						StateMenu::Input						()
{
	Slot += es_input->ButtonDown(0, ES_BUTTON_RIGHT) ? 1 : 0;
	Slot -= es_input->ButtonDown(0, ES_BUTTON_LEFT) ? 1 : 0;
	Slot = Utility::Clamp(Slot, 0, 8);

	char buffer[32];
	snprintf(buffer, 32, "Slot %d\n", Slot);
	((SummerfaceLabel*)GetInterface()->GetWindow("StateLabel"))->SetMessage(buffer);

	if(es_input->ButtonDown(0, ES_BUTTON_CANCEL))
	{
		return true;
	}

	if(es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
	{
		MDFNI_SelectState(Slot);
		MednafenEmu::DoCommand(0, 0, Load ? "DoLoadState" : "DoSaveState");
		return true;
	}

	return false;
}

bool						StateMenu::Draw							()
{
	char buffer[32];
	snprintf(buffer, 32, "SCRATCH%%%d", Slot);

	Texture* t = ImageManager::GetImage(buffer);

	if(t)
	{
		es_video->PresentFrame(t, Area(0, 0, States[Slot]->w, States[Slot]->h), 0, 0);
	}

	return false;
}

void						StateMenu::FillScratch					(uint32_t aSlot)
{
	char ibuffer[32];
	snprintf(ibuffer, 32, "SCRATCH%%%d", aSlot);

	if(States[aSlot] && States[aSlot]->gfx && States[aSlot]->w && States[aSlot]->w < 512 && States[aSlot]->h < 512 && ImageManager::GetImage(ibuffer))
	{
		Texture* tex = ImageManager::GetImage(ibuffer);
		uint32_t* texpix = tex->GetPixels();
		uint8_t* statepix = (uint8_t*)States[aSlot]->gfx;

		for(int k = 0; k != States[aSlot]->h; k ++)
		{
			for(int j = 0; j != States[aSlot]->w; j ++)
			{
				uint32_t r = statepix[0];
				uint32_t g = statepix[1];
				uint32_t b = statepix[2];
				texpix[k * tex->GetWidth() + j] = (r << 16) | (g << 8) | (b) | 0xFF000000;
				statepix += 3;
			}
		}
	}
}

