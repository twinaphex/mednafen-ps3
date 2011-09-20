ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

include $(CURDIR)/../Makefile-$(PLATFORM).config

TARGET		:=	libmupen64plus
TYPE		:=	lib
INCLUDE		:=	.. ../mednafen/src src/core/src src/core/src/api

CPPFLAGS	+=	-DMDFNPS3 -Dtrim=n64trim -Drom=n64rom

CXXFILES	+=	src/mednafen.cpp

#Audio plugin
CXXFILES	+=	src/audio/src/main-mdfn.cpp

#Input plugin
CXXFILES	+=	src/input/src/plugin-mdfn.cpp


#RSP Plugin
CXXFILES	+=	src/rsp-hle/src/ucode1.cpp \
				src/rsp-hle/src/ucode2.cpp \
				src/rsp-hle/src/ucode3.cpp \
				src/rsp-hle/src/ucode3mp3.cpp

CFILES		+=	src/rsp-hle/src/jpeg.c \
				src/rsp-hle/src/main-mdfn.c  

#Main emulator
CFILES		+=	src/core/src/main/zip/unzip.c \
				src/core/src/main/zip/zip.c \
				src/core/src/main/zip/ioapi.c

CFILES		+=	src/core/src/api/callbacks.c \
				src/core/src/api/common.c \
				src/core/src/api/config.c \
				src/core/src/api/frontend.c \
				src/core/src/api/vidext-mdfn.c
#				src/core/src/api/debugger.c

CFILES		+=	src/core/src/main/adler32.c \
				src/core/src/main/lirc.c \
				src/core/src/main/main.c \
				src/core/src/main/md5.c \
				src/core/src/main/rom.c \
				src/core/src/main/savestates.c \
				src/core/src/main/util.c
#				src/core/src/main/eventloop.c
#				src/core/src/main/ini_reader.c
#				src/core/src/main/cheat.c

CFILES		+=	src/core/src/memory/dma.c \
				src/core/src/memory/flashram.c \
				src/core/src/memory/memory.c \
				src/core/src/memory/n64_cic_nus_6105.c \
				src/core/src/memory/pif.c \
				src/core/src/memory/tlb.c

CFILES		+=	src/core/src/osal/dynamiclib_unix.c \
				src/core/src/osal/files_unix.c
#				src/core/src/osal/dynamiclib_win32.c
#				src/core/src/osal/files_win32.c

#CXXFILES	+=	src/core/src/osd/OGLFT.cpp \
#				src/core/src/osd/osd.cpp \
#				src/core/src/osd/screenshot.cpp

CFILES		+=	src/core/src/plugin/dummy_video.c \
				src/core/src/plugin/plugin.c
#				src/core/src/plugin/dummy_audio.c
#				src/core/src/plugin/dummy_rsp.c
#				src/core/src/plugin/dummy_input.c

CFILES		+=	src/core/src/r4300/bc.c \
				src/core/src/r4300/cop0.c \
				src/core/src/r4300/cop1.c \
				src/core/src/r4300/cop1_d.c \
				src/core/src/r4300/cop1_l.c \
				src/core/src/r4300/cop1_s.c \
				src/core/src/r4300/cop1_w.c \
				src/core/src/r4300/empty_dynarec.c \
				src/core/src/r4300/exception.c \
				src/core/src/r4300/interupt.c \
				src/core/src/r4300/profile.c \
				src/core/src/r4300/pure_interp.c \
				src/core/src/r4300/r4300.c \
				src/core/src/r4300/recomp.c \
				src/core/src/r4300/regimm.c \
				src/core/src/r4300/special.c \
				src/core/src/r4300/tlb.c

include $(CURDIR)/../Makefile-$(PLATFORM).base

