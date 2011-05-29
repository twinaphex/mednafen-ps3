ifeq ($(strip $(PLATFORM)),)
	PLATFORM = l1ght
endif

TARGET		:=	libpcsxr
ifeq ($(strip $(PLATFORM)),dll)
TYPE		:=	so
else
TYPE		:=	a
endif
SONAME		:=	libpcsxr
#SYMPREFIX	:=	pcsx
BUILD		:=	build
SOURCE		:=	src/libpcsxcore src src/mednafen src/mednafen/video_plugin src/mednafen/sound_plugin src/mednafen/input_plugin src/mednafen/cdrplugin
INCLUDE		:=	src src/libpcsxcore ../mednafen/src ../system
CFLAGS		:= 	-DNOPSXREC -DSTUPID_SPEED_TEST


#-DEMU_LOG=SysMessage -DPSXMEM_LOG=SysMessage -DPSXHW_LOG=SysMessage -DLOG_STDOUT -DPAD_LOG=SysMessage -DPSXDMA_LOG=SysMessage
#-D_WINDOWS

ifneq ($(BUILD)-$(PLATFORM),$(notdir $(CURDIR)))
	include $(CURDIR)/../Makefile-$(PLATFORM).base
else
	include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif

