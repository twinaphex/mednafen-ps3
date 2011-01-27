#!/bin/sh

export PLATFORM=dll
cd nestopia
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
cd ..

