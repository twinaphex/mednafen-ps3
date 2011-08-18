ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET			:=	libvbam
TYPE			:=	lib
SOURCE			:=	src src/src src/src/common src/src/apu src/src/gb src/src/gba 
INCLUDE			:=	.. ../mednafen/src src/src

CFLAGS			:=	-DFINAL_VERSION -DSDL -DNO_PNG -DHAVE_ZUTIL_H -D_SZ_ONE_DIRECTORY -D_LZMA_IN_CB -D_LZMA_OUT_READ -DNO_DEBUGGER -DC_CORE -DMDFNPS3 -DNO_LINK
CXXFLAGS		:=	-DFINAL_VERSION -DSDL -DNO_PNG -DHAVE_ZUTIL_H -D_SZ_ONE_DIRECTORY -D_LZMA_IN_CB -D_LZMA_OUT_READ -DNO_DEBUGGER -DC_CORE -DMDFNPS3 -DNO_LINK

ifeq ($(strip $(PLATFORM)),cell)
	#Quick fix for systemDebug token collision...
	CPPFLAGS	+=	-DsystemDebug=systemDebugVBAM
endif


include $(CURDIR)/../Makefile-$(PLATFORM).base

