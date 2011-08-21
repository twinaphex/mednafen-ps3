ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libdesmume
TYPE		:=	lib
INCLUDE		:=	.. ../mednafen/src src/desmume
CPPFLAGS	+=	-DMDFNPS3 -DHAVE_LIBZ
CXXFILES	+=	src/mednafen.cpp																																											\
				src/desmume/addons.cpp				src/desmume/cp15.cpp			src/desmume/FIFO.cpp				src/desmume/NDSSystem.cpp		src/desmume/slot1.cpp								\
				src/desmume/firmware.cpp			src/desmume/matrix.cpp			src/desmume/armcpu.cpp				src/desmume/debug.cpp			src/desmume/mc.cpp									\
				src/desmume/rasterize.cpp			src/desmume/SPU.cpp				src/desmume/arm_instructions.cpp	src/desmume/readwrite.cpp		src/desmume/texcache.cpp							\
				src/desmume/bios.cpp				src/desmume/gfx3d.cpp			src/desmume/mic.cpp					src/desmume/render3D.cpp		src/desmume/thumb_instructions.cpp					\
				src/desmume/cheatSystem.cpp			src/desmume/driver.cpp			src/desmume/GPU.cpp					src/desmume/version.cpp			src/desmume/MMU.cpp									\
				src/desmume/rtc.cpp					src/desmume/wifi.cpp			src/desmume/path.cpp				src/desmume/common.cpp			src/desmume/emufile.cpp								\
				src/desmume/GPU_osd_stub.cpp		src/desmume/saves.cpp			src/desmume/movie.cpp				src/desmume/fs-linux.cpp															\
				\
				src/desmume/sndmdfn.cpp																																										\
				\
				src/desmume/metaspu/metaspu.cpp																																								\
				\
				src/desmume/addons/slot1_none.cpp    src/desmume/addons/slot1_retail_nand.cpp  src/desmume/addons/slot2_guitarGrip.cpp  src/desmume/addons/slot2_paddle.cpp									\
				src/desmume/addons/slot1_r4.cpp      src/desmume/addons/slot2_expMemory.cpp    src/desmume/addons/slot2_mpcf.cpp        src/desmume/addons/slot2_piano.cpp									\
				src/desmume/addons/slot1_retail.cpp  src/desmume/addons/slot2_gbagame.cpp      src/desmume/addons/slot2_none.cpp        src/desmume/addons/slot2_rumblepak.cpp								\
				\
				src/desmume/utils/datetime.cpp  src/desmume/utils/emufat.cpp  src/desmume/utils/md5.cpp   src/desmume/utils/vfat.cpp																		\
				src/desmume/utils/dlditool.cpp  src/desmume/utils/guid.cpp    src/desmume/utils/task.cpp  src/desmume/utils/xstring.cpp																		\
				\
				src/desmume/utils/libfat/cache.cpp      src/desmume/utils/libfat/fatdir.cpp                 src/desmume/utils/libfat/filetime.cpp           src/desmume/utils/libfat/lock.cpp				\
				src/desmume/utils/libfat/directory.cpp  src/desmume/utils/libfat/fatfile.cpp                src/desmume/utils/libfat/libfat.cpp             src/desmume/utils/libfat/partition.cpp			\
				src/desmume/utils/libfat/disc.cpp       src/desmume/utils/libfat/file_allocation_table.cpp  src/desmume/utils/libfat/libfat_public_api.cpp													\
				\
				src/desmume/utils/decrypt/crc.cpp  src/desmume/utils/decrypt/decrypt.cpp  src/desmume/utils/decrypt/header.cpp


include $(CURDIR)/../Makefile-$(PLATFORM).base

