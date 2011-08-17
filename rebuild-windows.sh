#!/bin/sh

export PLATFORM=windows
cd system
make clean
make -j3
cd ../nestopia
make clean
make -j3
cd ../stella
make clean
make -j3
cd ../gambatte
make clean
make -j3
cd ../vbam
make clean
make -j3
cd ../pcsxr
make clean
make -j3
cd ../mednafen
make clean
make -j3
cd ..

