#include <ps3_system.h>
#include "SecretMenu.h"

					L1ghtSecret::L1ghtSecret				() : SummerfaceList(Area(10, 10, 80, 80))
{
	AddItem(new SummerfaceItem("Show Log", ""));
	AddItem(new SummerfaceItem("Get New EBOOT", ""));

	SetNoDelete();
	UI = new Summerface("list", this);
}

					L1ghtSecret::~L1ghtSecret				()
{
	delete UI;
}

bool				L1ghtSecret::Input						()
{
	if(es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
	{
		if(GetSelected()->GetText() == "Show Log")
		{
			Summerface("Log", es_log).Do();
		}
		else if(GetSelected()->GetText() == "Get New EBOOT")
		{
			std::vector<std::string> bms;
			FileSelect* selecter = new FileSelect("EBOOT.BIN", bms, "", 0);
			std::string enumpath = selecter->GetFile();

			std::string filename = Enumerators::GetEnumerator(enumpath).ObtainFile(enumpath);

			FILE* ongl = fopen(filename.c_str(), "rb");
			if(!ongl)
			{
				ESSUB_Error("Couldn't open new EBOOT.BIN");
				return false;
			}

			fseek(ongl, 0, SEEK_END);
			uint32_t size = ftell(ongl);
			fseek(ongl, 0, SEEK_SET);

			uint8_t* data = (uint8_t*)malloc(size);;
			fread(data, size, 1, ongl);
			fclose(ongl);

			if(data[0] != 0x53 || data[1] != 0x43 || data[2] != 0x45)
			{
				free(data);
				ESSUB_Error("File not a valid self file?");
				return false;
			}

			FILE* ingl = fopen("/dev_hdd0/game/MDFN90002/USRDIR/EBOOT.BIN", "wb");
			if(!ingl)
			{
				free(data);
				ESSUB_Error("Couldn't open old EBOOT.BIN");
				return true;
			}

			fwrite(data, size, 1, ingl);
			fclose(ingl);

			free(data);

			ESSUB_Error("EBOOT.BIN updated!");
		}

		return true;
	}

	return SummerfaceList::Input();
}

void				L1ghtSecret::Do							()
{
	UI->Do();		
}

