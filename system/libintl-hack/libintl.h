/* Message catalogs for internationalization.
   Copyright (C) 1995-2002, 2004, 2005 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   This file is derived from the file libgettext.h in the GNU gettext package.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* Hack to use libes's SetText instead of proper gettext, won't work in any but the simplest cases! */

#ifndef _LIBINTL_H
#define _LIBINTL_H	1

#include "src/utility/SetText.h"
#define gettext(a) SETTEXT_GetText(a)

#endif /* libintl.h */

