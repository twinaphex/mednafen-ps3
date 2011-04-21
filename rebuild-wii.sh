#!/bin/sh

export PLATFORM=wii
cd system
make clean
make -j2
cd ../nestopia
make clean
make -j2
cd ../gambatte
make clean
make -j2
cd ../vbam
make clean
make -j2
cd ../pcsxr
make clean
make -j2
cd ../mednafen
make clean
make -j2 dol
cd ..

