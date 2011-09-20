CC = gcc
CFLAGS = -DUSE_GTK -Wall `gtk-config --cflags` -O3 -mcpu=athlon -ffast-math -funroll-loops -fomit-frame-pointer -msse -mfpmath=sse
#CFLAGS = -DUSE_GTK `gtk-config --cflags` -g
CXX=g++
CPPFLAGS=$(CFLAGS)
LD = g++
LDFLAGS =

OBJECTS = main.o \
	vi.o     \
	vi_SDL.o \
	rsp.o    \
	rdp.o    \
	tx.o     \
	rs.o     \
	tf.o     \
	cc.o     \
	bl.o 

all:	mupen64_soft_gfx.so instruction

mupen64_soft_gfx.so: $(OBJECTS)
	$(LD) -shared -Wl,-Bsymbolic `gtk-config --libs` `sdl-config --libs` $(LDFLAGS) -o $@ $(OBJECTS)
	strip --strip-all $@

instruction:
	$(warning please copy mupen64_soft_gfx.so in the plugins/ folder of the emulator)

.o: .c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(OBJECTS) $(ALL)

rebuild: clean $(ALL)
