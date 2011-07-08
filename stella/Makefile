ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libstella
TYPE		:=	lib
BUILD		:=	build
SOURCE		:=	src src/emulator/cart src/emulator/input src/emulator/system src/emulator/utility src/emulator/properties src/emulator
INCLUDE		:=	../mednafen/src src/emulator src/emulator/cart src/emulator/input src/emulator/system src/emulator/utility src/emulator/properties 

CFLAGS		:=	-DHAVE_INTTYPES
CXXFLAGS	:=	-DHAVE_INTTYPES

ifneq ($(BUILD)-$(PLATFORM),$(notdir $(CURDIR)))
	include $(CURDIR)/../Makefile-$(PLATFORM).base
else
	include $(CURDIR)/../../Makefile-$(PLATFORM).base
endif
