ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

TARGET		:=	libstella
TYPE		:=	lib
INCLUDE		:=	.. ../mednafen/src src/emulator src/emulator/cart src/emulator/input src/emulator/system src/emulator/utility src/emulator/properties 

CPPFLAGS	+=	-DHAVE_INTTYPES -DKeyboard=StellaKeyboard

CXXFILES	+=	src/mednafen.cpp

CXXFILES	+=	src/emulator/Console.cpp								\
				src/emulator/Settings.cpp								\
				src/emulator/SoundSDL.cpp

CXXFILES	+=	src/emulator/cart/Cart0840.cpp							\
				src/emulator/cart/Cart2K.cpp							\
				src/emulator/cart/Cart3E.cpp							\
				src/emulator/cart/Cart3F.cpp							\
				src/emulator/cart/Cart4A50.cpp							\
				src/emulator/cart/Cart4K.cpp							\
				src/emulator/cart/CartAR.cpp							\
				src/emulator/cart/Cart.cpp								\
				src/emulator/cart/CartCV.cpp							\
				src/emulator/cart/CartDPC.cpp							\
				src/emulator/cart/CartDPCPlus.cpp						\
				src/emulator/cart/CartE0.cpp							\
				src/emulator/cart/CartE7.cpp							\
				src/emulator/cart/CartEF.cpp							\
				src/emulator/cart/CartEFSC.cpp							\
				src/emulator/cart/CartF0.cpp							\
				src/emulator/cart/CartF4.cpp							\
				src/emulator/cart/CartF4SC.cpp							\
				src/emulator/cart/CartF6.cpp							\
				src/emulator/cart/CartF6SC.cpp							\
				src/emulator/cart/CartF8.cpp							\
				src/emulator/cart/CartF8SC.cpp							\
				src/emulator/cart/CartFA.cpp							\
				src/emulator/cart/CartFE.cpp							\
				src/emulator/cart/CartMC.cpp							\
				src/emulator/cart/CartSB.cpp							\
				src/emulator/cart/CartUA.cpp							\
				src/emulator/cart/CartX07.cpp							\
				src/emulator/cart/Thumbulator.cpp

CXXFILES	+=	src/emulator/input/AtariVox.cpp							\
				src/emulator/input/Booster.cpp							\
				src/emulator/input/Control.cpp							\
				src/emulator/input/Driving.cpp							\
				src/emulator/input/Genesis.cpp							\
				src/emulator/input/Joystick.cpp							\
				src/emulator/input/Keyboard.cpp							\
				src/emulator/input/KidVid.cpp							\
				src/emulator/input/MT24LC256.cpp						\
				src/emulator/input/Paddles.cpp							\
				src/emulator/input/SaveKey.cpp							\
				src/emulator/input/Switches.cpp							\
				src/emulator/input/TrackBall.cpp

CXXFILES	+=	src/emulator/system/M6502.cpp							\
				src/emulator/system/M6532.cpp							\
				src/emulator/system/NullDev.cpp							\
				src/emulator/system/System.cpp							\
				src/emulator/system/TIA.cpp								\
				src/emulator/system/TIASnd.cpp							\
				src/emulator/system/TIATables.cpp

CXXFILES	+=	src/emulator/utility/MD5.cpp							\
				src/emulator/utility/Random.cpp							\
				src/emulator/utility/Serializer.cpp

CXXFILES	+=	src/emulator/properties/Props.cpp						\
				src/emulator/properties/PropsSet.cpp


include $(CURDIR)/../Makefile-$(PLATFORM).base

