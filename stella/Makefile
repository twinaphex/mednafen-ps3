ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libstella
TYPE		:=	lib
SOURCE		:=	src src/emulator/cart src/emulator/input src/emulator/system src/emulator/utility src/emulator/properties src/emulator
INCLUDE		:=	../mednafen/src src/emulator src/emulator/cart src/emulator/input src/emulator/system src/emulator/utility src/emulator/properties 

CFLAGS		:=	-DHAVE_INTTYPES -DKeyboard=StellaKeyboard
CXXFLAGS	:=	-DHAVE_INTTYPES -DKeyboard=StellaKeyboard

include $(CURDIR)/../Makefile-$(PLATFORM).base

