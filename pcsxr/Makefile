ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

include $(CURDIR)/../Makefile-$(PLATFORM).config

TARGET		:=	libpcsxr
TYPE		:=	lib
INCLUDE		:=	.. src src/libpcsxcore ../mednafen/src

CPPFLAGS	+= -DMDFNPS3 -DPACKAGE_VERSION=\"0.9.18\"

CXXFILES	+=	src/mednafen.cpp

CFILES		+=	src/libpcsxcore/cdrom.c									\
				src/libpcsxcore/cheat.c									\
				src/libpcsxcore/decode_xa.c								\
				src/libpcsxcore/disr3000a.c								\
				src/libpcsxcore/gpu.c									\
				src/libpcsxcore/gte.c									\
				src/libpcsxcore/mdec.c									\
				src/libpcsxcore/misc.c									\
				src/libpcsxcore/plugins.c								\
				src/libpcsxcore/ppf.c									\
				src/libpcsxcore/psxbios.c								\
				src/libpcsxcore/psxcommon.c								\
				src/libpcsxcore/psxcounters.c							\
				src/libpcsxcore/psxdma.c								\
				src/libpcsxcore/psxhle.c								\
				src/libpcsxcore/psxhw.c									\
				src/libpcsxcore/psxinterpreter.c						\
				src/libpcsxcore/psxmem.c								\
				src/libpcsxcore/r3000a.c								\
				src/libpcsxcore/sio.c									\
				src/libpcsxcore/spu.c

CFILES		+=	src/mednafen/plugins.c

CFILES		+=	src/mednafen/video_plugin/globals.c						\
				src/mednafen/video_plugin/gpu.c							\
				src/mednafen/video_plugin/prim.c						\
				src/mednafen/video_plugin/soft.c

CFILES		+=	src/mednafen/sound_plugin/dma.c							\
				src/mednafen/sound_plugin/externals.c					\
				src/mednafen/sound_plugin/freeze.c						\
				src/mednafen/sound_plugin/registers.c					\
				src/mednafen/sound_plugin/spu.c

CXXFILES	+=	src/mednafen/sound_plugin/mdfnsnd.cpp

CFILES		+=	src/mednafen/input_plugin/analog.c						\
				src/mednafen/input_plugin/pad.c

CXXFILES	+=	src/mednafen/cdrplugin/cdr.cpp

ifeq ($(strip $(PLATFORM)),cell)
	CPPFLAGS+=	-DNOPSXREC
else
	SOURCE	+= 	src/libpcsxcore/ix86_64 src/libpcsxcore/ix86
endif

include $(CURDIR)/../Makefile-$(PLATFORM).base

