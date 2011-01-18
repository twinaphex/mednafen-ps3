ifeq ($(strip $(PLATFORM)),)
	PLATFORM = l1ght
endif

TARGET		:=	libpcsxr
TYPE		:=	a
SYMPREFIX	:=	pcsx
BUILD		:=	build
SOURCE		:=	src/libpcsxcore src/libpcsxcore/ix86_64 src src/mednafen src/mednafen/video_plugin src/mednafen/sound_plugin src/mednafen/input_plugin src/mednafen/cdrplugin
INCLUDE		:=	src src/libpcsxcore ../mednafen/src ../system
CFLAGS		:= -fgnu89-inline

#-D_WINDOWS

ifneq ($(BUILD),$(notdir $(CURDIR)))
	include $(CURDIR)/../Makefile-$(PLATFORM).base
else
	include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif
