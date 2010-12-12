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

#include "video-common.h"

#include <zlib.h>

static int WritePNGChunk(FILE *fp, uint32 size, const char *type, const uint8 *data)
{
 uint32 crc;

 uint8 tempo[4];

 tempo[0]=size>>24;
 tempo[1]=size>>16;
 tempo[2]=size>>8;
 tempo[3]=size;

 if(fwrite(tempo,4,1,fp)!=1)
  return 0;
 if(fwrite(type,4,1,fp)!=1)
  return 0;

 if(size)
  if(fwrite(data,1,size,fp)!=size)
   return 0;

 crc = crc32(0,(uint8 *)type,4);
 if(size)
  crc = crc32(crc,data,size);

 tempo[0]=crc>>24;
 tempo[1]=crc>>16;
 tempo[2]=crc>>8;
 tempo[3]=crc;

 if(fwrite(tempo,4,1,fp)!=1)
  return 0;
 return 1;
}

int MDFN_SavePNGSnapshot(const char *fname, const MDFN_Surface *src, const MDFN_Rect *rect, const MDFN_Rect *LineWidths)
{
 FILE *pp=NULL;
 uint8 *compmem = NULL;
 uLongf compmemsize;
 int png_width;

 if(LineWidths[0].w != ~0)
 {
  png_width = 0;

  for(int y = 0; y < rect->h; y++)
  {
   if(LineWidths[rect->y + y].w > png_width)
    png_width = LineWidths[rect->y + y].w;
  }
 }
 else
  png_width = rect->w;

 if(!png_width)
  return(0);

 compmemsize = (uLongf)( (rect->h * (png_width + 1) * 3 * 1.001 + 1) + 12 );

 if(!(compmem=(uint8 *)MDFN_malloc(compmemsize, _("PNG compression buffer"))))
  return 0;

 if(!(pp=fopen(fname, "wb")))
 {
  return 0;
 }


 {
  static uint8 header[8]={137,80,78,71,13,10,26,10};
  if(fwrite(header,8,1,pp)!=1)
   goto PNGerr;
 }

 {
  uint8 chunko[13];

  chunko[0] = png_width >> 24;		// Width
  chunko[1] = png_width >> 16;
  chunko[2] = png_width >> 8;
  chunko[3] = png_width;

  chunko[4] = rect->h >> 24;		// Height
  chunko[5] = rect->h >> 16;
  chunko[6] = rect->h >> 8;
  chunko[7] = rect->h;

  chunko[8]=8;				// 8 bits per sample(24 bits per pixel)
  chunko[9]=2;				// Color type; RGB triplet
  chunko[10]=0;				// compression: deflate
  chunko[11]=0;				// Basic adapative filter set(though none are used).
  chunko[12]=0;				// No interlace.

  if(!WritePNGChunk(pp,13,"IHDR",chunko))
   goto PNGerr;
 }

 // pHYs chunk
 #if 0
 {
  uint8 chunko[9];
  uint32 ppx, ppy;

  //ppx = png_width / MDFNGameInfo->nominal_width;
  //ppy = 1;	//rect->h / rect->h
  
  ppx = png_width;
  ppy = MDFNGameInfo->nominal_width;

  MDFN_en32msb(&chunko[0], ppx);
  MDFN_en32msb(&chunko[4], ppy);
  
  //printf("%08x %08x, %04x %04x\n", ppx, ppy, *(uint32 *)&chunko[0], *(uint32 *)&chunko[4]);

  chunko[8] = 0;

  if(!WritePNGChunk(pp, 9, "pHYs", chunko))
   goto PNGerr;
 }
 #endif

 {
  uint8 *tmp_buffer;
  uint8 *tmp_inc;
  tmp_inc = tmp_buffer = (uint8 *)MDFN_malloc((png_width * 3 + 1) * rect->h, _("PNG compression buffer"));

  if(!tmp_inc)
   goto PNGerr;

  for(int y = 0; y < rect->h; y++)
  {
   *tmp_inc = 0;
   tmp_inc++;
   int line_width = rect->w;
   int x_base = rect->x;

   if(LineWidths[0].w != ~0)
   {
    line_width = LineWidths[y + rect->y].w;
    x_base = LineWidths[y + rect->y].x;
   }

   for(int x = 0; x < line_width; x++)
   {
    int r, g, b;

    if(src->format.bpp == 16)
     src->DecodeColor(src->pixels16[(y + rect->y) * src->pitchinpix + (x + x_base)], r, g, b);
    else
     src->DecodeColor(src->pixels[(y + rect->y) * src->pitchinpix + (x + x_base)], r, g, b);

    tmp_inc[0] = r;
    tmp_inc[1] = g;
    tmp_inc[2] = b;
    tmp_inc += 3;
   }

   for(int x = line_width; x < png_width; x++)
   {
    tmp_inc[0] = tmp_inc[1] = tmp_inc[2] = 0;
    tmp_inc += 3;
   }
  }

  if(compress(compmem, &compmemsize, tmp_buffer, rect->h * (png_width * 3 + 1))!=Z_OK)
  {
   if(tmp_buffer) free(tmp_buffer);
   goto PNGerr;
  }
  if(tmp_buffer) free(tmp_buffer);
  if(!WritePNGChunk(pp,compmemsize,"IDAT",compmem))
   goto PNGerr;
 }
 if(!WritePNGChunk(pp,0,"IEND",0))
  goto PNGerr;

 free(compmem);
 fclose(pp);

 return 1;

 PNGerr:
 if(compmem)
  free(compmem);
 if(pp)
  fclose(pp);
 return(0);
}
