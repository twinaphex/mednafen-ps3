#ifndef NEST_SETTINGS_H_MDFN
#define NEST_SETTINGS_H_MDFN

namespace nestMDFN
{
	struct								NestSettingsList
	{
		bool							NeedRefresh;
		bool							ClipSides;
		uint32_t						FavoredMachine;
		uint32_t						ScanLineStart;
		uint32_t						ScanLineEnd;
		std::string						FDSBios;
		bool							EnableNTSC;
		uint32_t						NTSCMode;
		bool							DisableSpriteLimit;

		//
		uint32_t						FPS;
	};

	void								GetSettings						(const char* aName = 0);

	extern NestSettingsList				NestopiaSettings;
	extern MDFNSetting					NestSettings[];
}

#endif


