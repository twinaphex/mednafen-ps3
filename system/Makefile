ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

include $(CURDIR)/Makefile-$(PLATFORM).config
include $(CURDIR)/../Makefile-$(PLATFORM).config

TARGET			:=	libemusys
TYPE			:=	lib

ifeq ($(strip $(USE_SOUNDTOUCH)),yes)
	CXXFILES	+=	src/thirdparty/soundtouch/source/SoundTouch/AAFilter.cpp \
					src/thirdparty/soundtouch/source/SoundTouch/BPMDetect.cpp \
					src/thirdparty/soundtouch/source/SoundTouch/PeakFinder.cpp \
					src/thirdparty/soundtouch/source/SoundTouch/RateTransposer.cpp \
					src/thirdparty/soundtouch/source/SoundTouch/SoundTouch.cpp \
					src/thirdparty/soundtouch/source/SoundTouch/FIFOSampleBuffer.cpp \
					src/thirdparty/soundtouch/source/SoundTouch/FIRFilter.cpp \
					src/thirdparty/soundtouch/source/SoundTouch/TDStretch.cpp \
					src/thirdparty/soundtouch/source/SoundTouch/cpu_detect_x86_gcc.cpp

	CPPFLAGS	+=	-DES_USESOUNDTOUCH
	INCLUDE		+=	src/thirdparty/soundtouch/include
endif

ifeq ($(strip $(USE_FEX)),yes)
	CXXFILES	+=	src/thirdparty/fex/fex/Binary_Extractor.cpp \
					src/thirdparty/fex/fex/Data_Reader.cpp \
					src/thirdparty/fex/fex/Gzip_Extractor.cpp \
					src/thirdparty/fex/fex/Zip7_Extractor.cpp \
					src/thirdparty/fex/fex/blargg_common.cpp \
					src/thirdparty/fex/fex/fex.cpp \
					src/thirdparty/fex/fex/Gzip_Reader.cpp \
					src/thirdparty/fex/fex/Zip_Extractor.cpp \
					src/thirdparty/fex/fex/blargg_errors.cpp \
					src/thirdparty/fex/fex/File_Extractor.cpp \
					src/thirdparty/fex/fex/Rar_Extractor.cpp \
					src/thirdparty/fex/fex/Zlib_Inflater.cpp

	CFILES		+=	src/thirdparty/fex/7z_C/7zAlloc.c \
					src/thirdparty/fex/7z_C/7zDecode.c \
					src/thirdparty/fex/7z_C/7zIn.c \
					src/thirdparty/fex/7z_C/Bcj2.c \
					src/thirdparty/fex/7z_C/7zBuf.c \
					src/thirdparty/fex/7z_C/7zExtract.c \
					src/thirdparty/fex/7z_C/7zItem.c \
					src/thirdparty/fex/7z_C/Bra86.c \
					src/thirdparty/fex/7z_C/7zCrc.c \
					src/thirdparty/fex/7z_C/7zHeader.c \
					src/thirdparty/fex/7z_C/7zStream.c \
					src/thirdparty/fex/7z_C/LzmaDec.c


	CPPFLAGS	+=	-DES_USEFEX
	INCLUDE		+=	src/thirdparty/fex
endif



SIMPLEINIFILES	:=	src/thirdparty/simpleini/ConvertUTF.c
UTF8DECODEFILES	:=	src/thirdparty/utf8_decode/utf8_decode.c

CFILES			+=	$(SIMPLEINIFILES) $(UTF8DECODEFILES)
SOURCE			:=	src src/GUI opengl_common src/utility src/utility/Files 
INCLUDE			+=	. src $(PLATFORM)  

include $(CURDIR)/../Makefile-$(PLATFORM).base

