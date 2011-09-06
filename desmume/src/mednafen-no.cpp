#include <stdarg.h>

//PS3 Includes
#include <cell/pad.h>

//Desmume includes
#define SFORMAT dsSFORMAT

#include "MMU.h"
#include "NDSSystem.h"
#include "debug.h"
#include "sndsdl.h"
#include "render3D.h"
#include "rasterize.h"
#include "saves.h"
#include "firmware.h"
#include "GPU_osd.h"
#include "addons.h"

//Desume interface
volatile bool								execute = true;

SoundInterface_struct*						SNDCoreList[] =
{
	&SNDDummy,
	NULL
};

GPU3DInterface*								core3DList[] =
{
	&gpu3DRasterize,
	NULL
};

static bool									OneScreen = false;
static bool									TopScreen = true;
static bool									HoldingScreenButton = false;
static bool									ScreenGap = false;
static bool									NeedScreenClear = false;


#include <es_system.h>
#include "src/utility/TextViewer.h"
#include "src/utility/Files/FileSelect.h"
#include "src/utility/Files/ArchiveList.h"


template<int rS, int gS, int bS, int rD, int gD, int bD, int sMul, int sMask, int dMul, int dAdd, typename sType>
inline void __attribute((always_inline))		BlitSwiz					(uint32_t* aTarget, const sType* aSource, uint32_t aWidth, uint32_t aHeight, uint32_t aPixelPitch, uint32_t aXOffset = 0, uint32_t aYOffset = 0)
{
	uint32_t* target = aTarget + (aXOffset) + (aYOffset * aPixelPitch);

	for(int i = 0; i != aHeight; i ++)
	{
		for(int j = 0; j != aWidth; j ++)
		{
			sType source = aSource[i * aPixelPitch + j];

			uint32_t r = ((source >> (rS * sMul)) & sMask) << (rD * dMul + dAdd);
			uint32_t g = ((source >> (gS * sMul)) & sMask) << (gD * dMul + dAdd);
			uint32_t b = ((source >> (bS * sMul)) & sMask) << (bD * dMul + dAdd);
			target[i * aPixelPitch + j] = r | g | b;
		}
	}
}


template<int rS, int gS, int bS, int rD, int gD, int bD>
inline void __attribute((always_inline))		BlitRGB15					(uint32_t* aTarget, const uint16_t* aSource, uint32_t aWidth, uint32_t aHeight, uint32_t aPixelPitch, uint32_t aXOffset = 0, uint32_t aYOffset = 0)
{
	BlitSwiz<rS, gS, bS, rD, gD, bD, 5, 0x1F, 8, 3, uint16_t>(aTarget, aSource, aWidth, aHeight, aPixelPitch, aXOffset, aYOffset);
}

void			RefreshInputs	()
{
	//Note: Input setup done by libsystem

	CellPadData PadState;
	cellPadGetData(0, &PadState);

	if(PadState.len)
	{
		NDS_beginProcessingInput();
		UserButtons& input = NDS_getProcessingUserInput().buttons;

		input.G = 0; // debug
		input.E = (PadState.button[3] & CELL_PAD_CTRL_R1) ? 1 : 0; // right shoulder
		input.W = (PadState.button[3] & CELL_PAD_CTRL_L1) ? 1 : 0; // left shoulder
		input.X = (PadState.button[3] & CELL_PAD_CTRL_TRIANGLE) ? 1 : 0;
		input.Y = (PadState.button[3] & CELL_PAD_CTRL_SQUARE) ? 1 : 0;
		input.A = (PadState.button[3] & CELL_PAD_CTRL_CIRCLE) ? 1 : 0;
		input.B = (PadState.button[3] & CELL_PAD_CTRL_CROSS) ? 1 : 0;
		input.S = (PadState.button[2] & CELL_PAD_CTRL_START) ? 1 : 0; // start
		input.T = (PadState.button[2] & CELL_PAD_CTRL_SELECT) ? 1 : 0; // select
		input.U = (PadState.button[2] & CELL_PAD_CTRL_UP) ? 1 : 0; // up
		input.D = (PadState.button[2] & CELL_PAD_CTRL_DOWN) ? 1 : 0; // down
		input.L = (PadState.button[2] & CELL_PAD_CTRL_LEFT) ? 1 : 0; // left
		input.R = (PadState.button[2] & CELL_PAD_CTRL_RIGHT) ? 1 : 0; // right
		input.F = (PadState.button[2] & CELL_PAD_CTRL_R3) ? 1 : 0; // lid

		NDS_endProcessingInput();
	}
}

int				main			(int argc, char* argv)
{
	InitES();
	FileSelect* FileChooser = new FileSelect("Select ROM", "", "", 0);

	std::string file = FileChooser->GetFile();

	if(file.empty())
	{
		return 1;
	}

	//Setup desmume
	struct NDS_fw_config_data fw_config;
	NDS_FillDefaultFirmwareConfigData(&fw_config);

	addonsChangePak(NDS_ADDON_NONE);
	NDS_Init();
	NDS_CreateDummyFirmware(&fw_config);
	NDS_3D_ChangeCore(0);
	SPU_ChangeSoundCore(SNDCORE_DUMMY, 735 * 2);		
	backup_setManualBackupType(MC_TYPE_AUTODETECT);

	//Load ROM
	NDS_LoadROM(file.c_str());

	Texture* tex = ESVideo::CreateTexture(256, 192 * 2);
	ESVideo::UpdatePresentArea(2, 5);

	while(!WantToDie())
	{
		RefreshInputs();

		NDS_exec<false>();
		SPU_Emulate_user();

		BlitRGB15<0, 1, 2, 2, 1, 0>(tex->Map(), (const uint16_t*)GPU_screen, 256, 192 * 2, 256);
		tex->Unmap();
		ESVideo::PresentFrame(tex, Area(0, 0, 256, 192 * 2));
		ESVideo::Flip();
	}
}


