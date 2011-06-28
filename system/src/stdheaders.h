#pragma once

//Include System headers, these are common between platforms, and should remain static between builds
//C
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//C++
#include <vector>
#include <stack>
#include <list>
#include <map>
#include <exception>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

//Smart ptr
#ifndef USE_CPP0X_SMARTPTR
#include <boost/smart_ptr.hpp>
#include <boost/make_shared.hpp>
namespace smartptr = boost;
#else
#include <memory>
namespace smartptr = std;
#endif


//Shared ptr helper macro
#define DEFINE_PTR_TYPE(a)						\
	class	a;									\
	typedef smartptr::shared_ptr<a>	a##_Ptr;	\
	typedef smartptr::weak_ptr<a>	a##_WeakPtr;


//Depends
#include <png.h>

extern "C"
{
	#include <ft2build.h>
	#include FT_FREETYPE_H
	#include FT_BITMAP_H
	#include FT_GLYPH_H
}

