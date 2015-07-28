TODO: Document building windows native, document building requirements for mingw

# Required Libraries #
  * All platforms
    * **zlib** 1.2.5 (zlib.net)
    * **libpng** 1.2 (libpng.org)
    * **freetype2** (freetype.org)

  * Windows (SDL) and Linux:
    * **SDL** (libsdl.org)
    * **SDL\_net** (libsdl.org/projects/SDL\_net/)
    * **nvidia CG toolkit**


---


# Building with the PS3 SDK #
## Environment ##
> The following environment variables can be used to control the build
  * **CELL\_SDK**: Set to the root path of your cell sdk installation
  * **FREETYPECONFIG**: Set to the path to the freetype-config script, by default assume freetype was installed to **$CELL\_SDK/target/ppu**
  * **PLATFORM**: Must be set to **cell**
  * **PREFIX**: Must be set to **ppu-lv2-** or not set

## Installing requirements ##
The commands below will install the dependencies into the $CELL\_SDK/target/ppu directory. If you wish to keep them separate you should change the --prefix options to the configure scripts, and modify Makefile-cell.base to include your new paths.

### zlib ###
```
CC=ppu-lv2-gcc ./configure --prefix=$CELL_SDK/target/ppu --static

The line 'cp $(SHAREDLIBV) $(DESTDIR)$(sharedlibdir)', under the 'install-libs:' section, must
be manually deleted from the Makefile.

make
make install
```

When building zlib 1.2.5 with a Windows host the following lines must be deleted from the configure script before it is run. You can use most text editors to do this, however the standard notepad edtior will not work, WordPad can be used if needed.
```
echo "Please use win32/Makefile.gcc instead."
exit 1
```

### libpng ###
```
CC=ppu-lv2-gcc ./configure --prefix=$CELL_SDK/target/ppu --host=powerpc --disable-dependency-tracking
make
make install
```

### freetype2 ###
```
CC=ppu-lv2-gcc ./configure --prefix=$CELL_SDK/target/ppu --host=powerpc
make
make install
```

## Building mednafen-ps3 ##
In the root directory run **./rebuild-cell.sh** to clean and rebuild all needed parts. Alternatively you must run make in **system**, **nestopia**, **gambatte**, **vbam**, **stella** and **pcsx** before you can build **mednafen**.

Run **make pkg** in the mednafen subdirectory to build installable packages.


---


# Building SDL for linux and Windows #
## The SDL version cannot be built for the PS3 as it requires OpenGL ##

## Environment ##
> The following environment variables can be used to control the build
  * **FREETYPECONFIG**: Set to the path to the freetype-config script, by default uses **`which freetype-config`**.
  * **SDLCONFIG** Set to the path to the sdl-config script, by default uses **`which sdl-config`**.
  * **PLATFORM**: Must be set to **sdl**.
  * **TARGETPLATFORM**: Set to **windows** to use win32 style opengl libraries and a .exe extension.
  * **PREFIX**: Typically not set, but may be useful when cross compiling.

## Requirements ##
On ubuntu the required libraries can be installed with the command:
```
sudo apt-get install zlib1g-dev libpng12-dev libfreetype6-dev libsdl1.2-dev libsdl-net1.2-dev nvidia-cg-toolkit
```

## Building ##
In the root directory run ./rebuild-sdl.sh to clean and rebuild all needed parts. Alternatively you must run make in **system**, **nestopia**, **gambatte**, **vbam**, **stella** and **pcsx** before you can build **mednafen**.

## Running ##
Before mednafen-ps3 can be run the contents of **mednafen/pkg/USRDIR** must be copied into place. On ubuntu this is **~/.mednafen/** (eg ~/.mednafen/mednafen and ~/.mednafen/assets) on Windows 7 it would be **C:\Users\AppData\Roaming\mednafen**.