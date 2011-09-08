#include <mednafen/mednafen.h>
#include <mednafen/general.h>

#include <fstream>
#include "core/api/NstApiEmulator.hpp"
#include "core/api/NstApiFds.hpp"
#include "core/api/NstApiCartridge.hpp"

using namespace Nes;
using namespace Nes::Api;

#include "mednafen.h"
#include "fileio.h"
#include "settings.h"

namespace	nestMDFN
{
	void							LoadCartDatabase				()
	{
		if(!Cartridge::Database(Nestopia).IsLoaded())
		{
			std::ifstream database(MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, "NstDatabase.xml").c_str(), std::ifstream::binary);

			if(database.is_open())
			{
				if(NES_FAILED(Cartridge::Database(Nestopia).Load(database)))
				{
					MDFND_Message("nest: Couldn't load cartridge database");
				}
				else if(NES_FAILED(Cartridge::Database(Nestopia).Enable(true)))
				{
					MDFND_Message("nest: Couldn't enable cartridge database");
				}
				else
				{
					MDFND_Message("nest: Nestopia Cartridge Database opened");
				}
			}
			else
			{
				MDFND_Message("nest: Nestopia Cartridge Database not found");
			}
		}
	}

	void							LoadFDSBios						()
	{
		if(!Fds(Nestopia).HasBIOS())
		{
			std::ifstream bios(MDFN_MakeFName(MDFNMKF_FIRMWARE, 0, NestopiaSettings.FDSBios.c_str()).c_str(), std::ifstream::binary);

			if(bios.is_open())
			{
				if(NES_FAILED(Fds(Nestopia).SetBIOS(&bios)))
				{
					MDFND_Message("nest: Couldn't set FDS bios");
				}
				else
				{
					MDFND_Message("nest: FDS BIOS opened");
				}
			}
			else
			{
				MDFND_Message("nest: FDS BIOS not found");
			}
		}
	}

	Machine::Mode					GetSystemType					(const uint8_t* aCart, uint32_t aSize, uint32_t aFavored)
	{
		if(Cartridge::Database(Nestopia).IsLoaded())
		{
			Cartridge::Database::Entry dbentry = Cartridge::Database(Nestopia).FindEntry(aCart + 16, aSize - 16, (Nes::Api::Machine::FavoredSystem)NestopiaSettings.FavoredMachine);
			if(dbentry.GetSystem() == Cartridge::Profile::System::NES_PAL)
			{
				return Machine::PAL;
			}
			else
			{
				return Machine::NTSC;
			}
		}
		else
		{
			return Machine::NTSC;
		}
	}

	//Definitions
	FileExtensionSpecStruct	NestExtensions[] =
	{
		{".nes",	"iNES Format ROM Image"},
		{".nez",	"iNES Format ROM Image"},
		{".fds",	"Famicom Disk System Disk Image"},
		{".unf",	"UNIF Format ROM Image"},
		{".unif",	"UNIF Format ROM Image"},
		{0, 0}
	};
}


