/* Mednafen - Multi-system Emulator
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "mednafen.h"

//
// It only support exclusive locks for now, for possible portability issues.
//

bool MDFN_flock(FILE *fp)
{
#ifdef HAVE_FLOCK

#elif defined(HAVE__LOCKING)
 int failure;
 while((failure = _locking(fileno(fp), _LK_LOCK, _filelength(fileno(fp)) && errno == EDEADLOCK)

 return(0 == _locking(fileno(fp), 
#else
 return(0);
#endif
}

bool MDFN_funlock(FILE *fp)
{
#ifdef HAVE_FLOCK

#elif defined(HAVE__LOCKING)

#else
 return(0);
#endif

}
