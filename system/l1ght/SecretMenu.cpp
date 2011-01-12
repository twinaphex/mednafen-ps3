#include <ps3_system.h>
#include "SecretMenu.h"

					L1ghtSecret::L1ghtSecret				() : WinterfaceList("Shh, it's a secret", true, true, 0)
{
	Items.push_back(new ListItem("Show Log"));
	Items.push_back(new ListItem("Get New EBOOT"));

	SideItems.push_back(new InputListItem("Navigate", ES_BUTTON_UP));
	SideItems.push_back(new InputListItem("Run Command", ES_BUTTON_ACCEPT));
	SideItems.push_back(new InputListItem("Close Menu", ES_BUTTON_CANCEL));
}

					L1ghtSecret::~L1ghtSecret				()
{

}

bool				L1ghtSecret::Input						()
{
	if(es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
	{
		if(GetSelected()->GetText() == "Show Log")
		{
			es_log->Do();
		}
		else if(GetSelected()->GetText() == "Get New EBOOT")
		{
			std::vector<std::string> bms;
			FileSelect* selecter = new FileSelect("EBOOT.BIN", bms, "", 0);
			std::string enumpath = selecter->GetFile();

			if(enumpath.find("/EBOOT.BIN"))
			{
				std::string filename = Enumerators::GetEnumerator(enumpath).ObtainFile(enumpath);

				FILE* ongl = fopen(filename.c_str(), "rb");
				if(!ongl)
				{
					es_log->Log("Couldn't open new EBOOT.BIN");
					es_log->Do();
					exit(0);
				}

				fseek(ongl, 0, SEEK_END);
				uint32_t size = ftell(ongl);
				fseek(ongl, 0, SEEK_SET);

				uint8_t* data = (uint8_t*)malloc(size);;
				fread(data, size, 1, ongl);
				fclose(ongl);

				FILE* ingl = fopen("/dev_hdd0/game/MDFN90002/USRDIR/EBOOT.BIN", "wb");
				if(!ingl)
				{
					es_log->Log("Couldn't old open EBOOT.BIN");
					es_log->Do();
					exit(0);
				}

				fwrite(data, size, 1, ingl);
				fclose(ingl);

				free(data);

				es_log->Log("EBOOT.BIN updated, Leaving!");
				es_log->Do();
				exit(0);
			}

			es_log->Log("Not updating EBOOT.BIN");
			es_log->Do();
		}

		return true;
	}

	return WinterfaceList::Input();
}

