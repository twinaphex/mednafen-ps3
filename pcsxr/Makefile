.SUFFIXES:
ifeq ($(strip $(PLATFORM)),)
PLATFORM = l1ght
endif

TARGET		:=	libpcsxr
TYPE		:=	a
BUILD		:=	build
SOURCE		:=	src/libpcsxcore src src/mednafen src/mednafen/video_plugin src/mednafen/sound_plugin src/mednafen/input_plugin src/mednafen/cdrplugin
INCLUDE		:=	src src/libpcsxcore ../mednafen/src ../system
DATA		:=	bin
LIBS		:=	
LIBDIRS		:=	
CFLAGS		:=-O2 -DNOPSXREC -fgnu89-inline
CXXFLAGS	:=-O2 -DNOPSXREC 

#-D_WINDOWS

ifneq ($(BUILD),$(notdir $(CURDIR)))
include $(CURDIR)/../Makefile-$(PLATFORM).base
else
include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif
