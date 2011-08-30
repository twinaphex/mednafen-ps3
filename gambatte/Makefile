ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libgambatte
TYPE		:=	lib
INCLUDE		:=	.. ../mednafen/src src/libgambatte/include src/common src/libgambatte/src ../system

CPPFLAGS	+=	-DMDFNPS3 -DHAVE_STDINT_H

CXXFILES	+=	src/mednafen.cpp

CXXFILES	+=	src/libgambatte/src/bitmap_font.cpp						\
				src/libgambatte/src/colorconversion.cpp					\
				src/libgambatte/src/cpu.cpp								\
				src/libgambatte/src/gambatte.cpp						\
				src/libgambatte/src/initstate.cpp						\
				src/libgambatte/src/interrupter.cpp						\
				src/libgambatte/src/memory.cpp							\
				src/libgambatte/src/rtc.cpp								\
				src/libgambatte/src/sound.cpp							\
				src/libgambatte/src/statesaver.cpp						\
				src/libgambatte/src/video.cpp

CXXFILES	+=	src/libgambatte/src/file/file.cpp

CXXFILES	+=	src/libgambatte/src/sound/channel1.cpp					\
				src/libgambatte/src/sound/channel2.cpp					\
				src/libgambatte/src/sound/channel3.cpp					\
				src/libgambatte/src/sound/channel4.cpp					\
				src/libgambatte/src/sound/duty_unit.cpp					\
				src/libgambatte/src/sound/envelope_unit.cpp				\
				src/libgambatte/src/sound/length_counter.cpp

CXXFILES	+=	src/libgambatte/src/video/basic_add_event.cpp			\
				src/libgambatte/src/video/break_event.cpp				\
				src/libgambatte/src/video/irq_event.cpp					\
				src/libgambatte/src/video/lyc_irq.cpp					\
				src/libgambatte/src/video/ly_counter.cpp				\
				src/libgambatte/src/video/m3_extra_cycles.cpp			\
				src/libgambatte/src/video/mode0_irq.cpp					\
				src/libgambatte/src/video/mode1_irq.cpp					\
				src/libgambatte/src/video/mode2_irq.cpp					\
				src/libgambatte/src/video/mode3_event.cpp				\
				src/libgambatte/src/video/sc_reader.cpp					\
				src/libgambatte/src/video/scx_reader.cpp				\
				src/libgambatte/src/video/sprite_mapper.cpp				\
				src/libgambatte/src/video/we.cpp						\
				src/libgambatte/src/video/we_master_checker.cpp			\
				src/libgambatte/src/video/wx_reader.cpp					\
				src/libgambatte/src/video/wy.cpp

CXXFILES	+=	src/common/resample/chainresampler.cpp					\
				src/common/resample/resamplerinfo.cpp					\
				src/common/resample/u48div.cpp

include $(CURDIR)/../Makefile-$(PLATFORM).base

