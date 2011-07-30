ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libemusys
TYPE		:=	lib
SOURCE		:=	$(PLATFORM) src opengl_common src/utility src/utility/Files src/utility/GUI src/utility/GUI/Summerface
SOURCE		+=	src/thirdparty/fex/fex src/thirdparty/fex/7z_C src/thirdparty/utf8_decode src/thirdparty/simpleini

CXXFILES	:=	src/thirdparty/soundtouch/source/SoundTouch/AAFilter.cpp src/thirdparty/soundtouch/source/SoundTouch/BPMDetect.cpp src/thirdparty/soundtouch/source/SoundTouch/PeakFinder.cpp \
				src/thirdparty/soundtouch/source/SoundTouch/RateTransposer.cpp src/thirdparty/soundtouch/source/SoundTouch/SoundTouch.cpp src/thirdparty/soundtouch/source/SoundTouch/FIFOSampleBuffer.cpp \
				src/thirdparty/soundtouch/source/SoundTouch/FIRFilter.cpp src/thirdparty/soundtouch/source/SoundTouch/TDStretch.cpp


INCLUDE		:=	. src src/thirdparty/fex src/thirdparty/soundtouch/include

#ifneq ($(strip $(NO_LUA)),1)
#	SOURCE	+=	src/lua
#	INCLUDE	+=	src/lua
#else
#	CXXFLAGS+=	-DNO_LUA
#endif

include $(CURDIR)/../Makefile-$(PLATFORM).base

