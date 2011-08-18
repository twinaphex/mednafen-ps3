ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libemusys
TYPE		:=	lib
SOURCE		:=	$(PLATFORM) src src/GUI opengl_common src/utility src/utility/Files 

SNDTOUCHFILES	:=	src/thirdparty/soundtouch/source/SoundTouch/AAFilter.cpp            src/thirdparty/soundtouch/source/SoundTouch/BPMDetect.cpp					\
					src/thirdparty/soundtouch/source/SoundTouch/PeakFinder.cpp			src/thirdparty/soundtouch/source/SoundTouch/RateTransposer.cpp				\
					src/thirdparty/soundtouch/source/SoundTouch/SoundTouch.cpp			src/thirdparty/soundtouch/source/SoundTouch/FIFOSampleBuffer.cpp			\
					src/thirdparty/soundtouch/source/SoundTouch/FIRFilter.cpp           src/thirdparty/soundtouch/source/SoundTouch/TDStretch.cpp					\
					src/thirdparty/soundtouch/source/SoundTouch/cpu_detect_x86_gcc.cpp

SIMPLEINIFILES	:=	src/thirdparty/simpleini/ConvertUTF.c

UTF8DECODEFILES	:=	src/thirdparty/utf8_decode/utf8_decode.c

FEXCFILES		:=	src/thirdparty/fex/7z_C/7zAlloc.c  src/thirdparty/fex/7z_C/7zDecode.c   src/thirdparty/fex/7z_C/7zIn.c      src/thirdparty/fex/7z_C/Bcj2.c		\
					src/thirdparty/fex/7z_C/7zBuf.c    src/thirdparty/fex/7z_C/7zExtract.c  src/thirdparty/fex/7z_C/7zItem.c    src/thirdparty/fex/7z_C/Bra86.c		\
					src/thirdparty/fex/7z_C/7zCrc.c    src/thirdparty/fex/7z_C/7zHeader.c   src/thirdparty/fex/7z_C/7zStream.c  src/thirdparty/fex/7z_C/LzmaDec.c

FEXCPPFILES		:=	src/thirdparty/fex/fex/Binary_Extractor.cpp  src/thirdparty/fex/fex/Data_Reader.cpp     src/thirdparty/fex/fex/Gzip_Extractor.cpp  src/thirdparty/fex/fex/Zip7_Extractor.cpp	\
					src/thirdparty/fex/fex/blargg_common.cpp     src/thirdparty/fex/fex/fex.cpp             src/thirdparty/fex/fex/Gzip_Reader.cpp     src/thirdparty/fex/fex/Zip_Extractor.cpp		\
					src/thirdparty/fex/fex/blargg_errors.cpp     src/thirdparty/fex/fex/File_Extractor.cpp  src/thirdparty/fex/fex/Rar_Extractor.cpp   src/thirdparty/fex/fex/Zlib_Inflater.cpp


CFILES			+=	$(FEXCFILES) $(SIMPLEINIFILES) $(UTF8DECODEFILES)
CXXFILES		+=	$(FEXCPPFILES) $(SNDTOUCHFILES)


INCLUDE		:=	. src src/thirdparty/fex src/thirdparty/soundtouch/include

include $(CURDIR)/../Makefile-$(PLATFORM).base

