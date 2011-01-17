#ifndef NEST_FILEIO_H_MDFN
#define NEST_FILEIO_H_MDFN

using namespace Nes;
using namespace Nes::Api;

namespace nestMDFN
{
	void							LoadCartDatabase				();
	void							LoadFDSBios						();

	Machine::Mode					GetSystemType					(const uint8_t* aCart, uint32_t aSize, uint32_t aFavored);

	extern FileExtensionSpecStruct	NestExtensions[];
}

#endif
