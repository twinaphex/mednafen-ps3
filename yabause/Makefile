ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libyabause
TYPE		:=	lib

CFILES		:=	src/yabause/bios.c src/yabause/cdbase.c src/yabause/cheat.c src/yabause/coffelf.c src/yabause/cs0.c src/yabause/cs1.c \
				src/yabause/cs2.c src/yabause/debug.c src/yabause/error.c src/yabause/m68kc68k.c src/yabause/m68kcore.c src/yabause/m68kd.c \
				src/yabause/memory.c src/yabause/movie.c src/yabause/netlink.c src/yabause/peripheral.c src/yabause/profile.c src/yabause/scsp.c \
				src/yabause/scu.c src/yabause/sh2core.c src/yabause/sh2d.c src/yabause/sh2idle.c src/yabause/sh2int.c src/yabause/sh2trace.c src/yabause/smpc.c \
				src/yabause/snddummy.c src/yabause/thr-dummy.c src/yabause/vdp1.c src/yabause/vdp2.c src/yabause/vdp2debug.c src/yabause/vidshared.c src/yabause/vidsoft.c \
				src/yabause/yabause.c src/yabause/permdfnjoy.c src/yabause/c68k/c68k.c src/yabause/c68k/c68kexec.c

CXXFILES	:=	src/mednafen.cpp src/yabause/cd-mdfn.cpp src/yabause/sndmdfn.cpp

INCLUDE		:=	.. ../mednafen/src src/yabause 

CFLAGS		+=	-DHAVE_C68K=1 -DMDFNPS3 -DVERSION=\"10\" -DHAVE_STDINT_H
CXXFLAGS	+=	-DHAVE_C68K=1 -DMDFNPS3 -DVERSION=\"10\" -DHAVE_STDINT_H

include $(CURDIR)/../Makefile-$(PLATFORM).base

