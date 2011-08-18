ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libpcsxr
TYPE		:=	lib
SOURCE		:=	src/libpcsxcore src src/mednafen src/mednafen/video_plugin src/mednafen/sound_plugin src/mednafen/input_plugin src/mednafen/cdrplugin
INCLUDE		:=	.. src src/libpcsxcore ../mednafen/src ../system

ifeq ($(strip $(PLATFORM)),cell)
	CFLAGS	:=	-DNOPSXREC
else
	SOURCE	+= 	src/libpcsxcore/ix86_64 src/libpcsxcore/ix86
endif

CPPFLAGS	+= -DMDFNPS3 -DPACKAGE_VERSION=\"0.9.16\"

include $(CURDIR)/../Makefile-$(PLATFORM).base

