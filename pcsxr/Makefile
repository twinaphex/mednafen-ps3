ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libpcsxr
TYPE		:=	lib
BUILD		:=	build
SOURCE		:=	src/libpcsxcore src src/mednafen src/mednafen/video_plugin src/mednafen/sound_plugin src/mednafen/input_plugin src/mednafen/cdrplugin
INCLUDE		:=	src src/libpcsxcore ../mednafen/src ../system
ifeq ($(strip $(PLATFORM)),cell)
	CFLAGS	:=	-DNOPSXREC
else
	SOURCE	+= 	src/libpcsxcore/ix86_64 src/libpcsxcore/ix86
endif



#-DEMU_LOG=SysMessage -DPSXMEM_LOG=SysMessage -DPSXHW_LOG=SysMessage -DLOG_STDOUT -DPAD_LOG=SysMessage -DPSXDMA_LOG=SysMessage
#-D_WINDOWS

ifneq ($(BUILD)-$(PLATFORM),$(notdir $(CURDIR)))
	include $(CURDIR)/../Makefile-$(PLATFORM).base
else
	include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif

