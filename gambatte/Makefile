ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

include $(CURDIR)/../Makefile-$(PLATFORM).config

TARGET		:=	libgambatte
TYPE		:=	lib
INCLUDE		:=	.. ../mednafen/src src/libgambatte/include src/common src/libgambatte/src

CPPFLAGS	+=	-DMDFNPS3 -DHAVE_STDINT_H

CXXFILES	+=	src/mednafen.cpp

CXXFILES	+=	src/libgambatte/src/bitmap_font.cpp \
				src/libgambatte/src/cpu.cpp \
				src/libgambatte/src/gambatte.cpp \
				src/libgambatte/src/initstate.cpp \
				src/libgambatte/src/interrupter.cpp \
				src/libgambatte/src/interruptrequester.cpp \
				src/libgambatte/src/memory.cpp \
				src/libgambatte/src/sound.cpp \
				src/libgambatte/src/state_osd_elements.cpp \
				src/libgambatte/src/statesaver.cpp \
				src/libgambatte/src/tima.cpp \
				src/libgambatte/src/video.cpp

CXXFILES	+=	src/libgambatte/src/file/file.cpp

CXXFILES	+=	src/libgambatte/src/sound/channel1.cpp \
				src/libgambatte/src/sound/channel2.cpp \
				src/libgambatte/src/sound/channel3.cpp \
				src/libgambatte/src/sound/channel4.cpp \
				src/libgambatte/src/sound/duty_unit.cpp \
				src/libgambatte/src/sound/envelope_unit.cpp \
				src/libgambatte/src/sound/length_counter.cpp

CXXFILES	+=	src/libgambatte/src/video/lyc_irq.cpp \
				src/libgambatte/src/video/ly_counter.cpp \
				src/libgambatte/src/video/next_m0_time.cpp \
				src/libgambatte/src/video/ppu.cpp \
				src/libgambatte/src/video/sprite_mapper.cpp

CXXFILES	+=	src/libgambatte/src/mem/cartridge.cpp \
				src/libgambatte/src/mem/memptrs.cpp \
				src/libgambatte/src/mem/rtc.cpp

CXXFILES	+=	src/common/resample/src/chainresampler.cpp \
				src/common/resample/src/i0.cpp \
				src/common/resample/src/makesinckernel.cpp \
				src/common/resample/src/resamplerinfo.cpp \
				src/common/resample/src/u48div.cpp


include $(CURDIR)/../Makefile-$(PLATFORM).base

