#!/bin/sh

cd system-l1ght
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
make -j2 vbam-objmangle
cd ../mednafen
make clean
make -j2
make pkg
cd ..
mv mednafen/mednafen.pkg .
