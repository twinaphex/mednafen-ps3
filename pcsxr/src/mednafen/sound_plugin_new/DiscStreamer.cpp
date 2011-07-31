/***************************************************************************
                            xa.c  -  description
                             -------------------
    begin                : Wed May 15 2002
    copyright            : (C) 2002 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

#include "stdafx.h"
#include "SPU.h"
#include "registers.h"
#include "DiscStreamer.h"

void					PSX::SPU::DiscStreamer::SetVolume						(bool aRight, int32_t aValue)
{
	Volume[aRight ? 1 : 0] = aValue;

	//Tell the emulator
	if(cddavCallback)
	{
		cddavCallback(aRight, aValue);
	}
}

void					PSX::SPU::DiscStreamer::Mix								(int32_t* aLeft, int32_t* aRight)
{
	int16_t leftChannel = 0, rightChannel = 0;

	//Fetch a frame from the buffer
	if(Play != Feed)
	{
		leftChannel = Data[Play * 2];
		rightChannel = Data[Play * 2 + 1];
		Play = (Play + 1) & 0xFFFF;
	}

	//Store samples into SPU memory
	Parent.StoreDecoded(0, leftChannel);
	Parent.StoreDecoded(1, rightChannel);

	//Apply volume
	leftChannel = CLAMP16((leftChannel * Volume[0]) / 0x8000);
	rightChannel = CLAMP16((rightChannel * Volume[1]) / 0x8000);

	//Reverb
	//TODO: Is this ever used?
	if(Parent.Control & CTRL_CD_REVERB)
	{
//		StoreREVERB_CD(leftChannel, rightChannel, 0);
	}

	//Mix
	if(Parent.Control & CTRL_CD_PLAY)
	{
		*aLeft += leftChannel;
		*aRight += rightChannel;
	}
}

void					PSX::SPU::DiscStreamer::FeedXA							(xa_decode_t* aData)
{
	if(aData && aData->freq)
	{
		//Get data size
		//HACK: Magic number
		int32_t dataSize = ((44100 * aData->nsamples) / aData->freq);

		if(dataSize)
		{
			//Calculate frequency values
			int32_t spos = 0x10000L;
			int32_t sinc = (aData->nsamples << 16) / dataSize;

			//Fill stream
			for(int i = 0; i != dataSize; i ++)
			{
				int16_t leftSample, rightSample;

				while(spos >= 0x10000L)
				{
					leftSample = aData->pcm[aData->stereo ? (i * 2 + 0) : i];
					rightSample = aData->pcm[aData->stereo ? (i * 2 + 1) : i];
					spos -= 0x10000L;
				}

				Data[Feed * 2 + 0] = leftSample;
				Data[Feed * 2 + 1] = rightSample;
				Feed = (Feed + 1) & 0xFFFF;

				if(Feed == Play)
				{
					Feed = Play ? Play - 1 : Feed;
					break;
				}

				spos += sinc;
			}
		}
	}
}

void					PSX::SPU::DiscStreamer::FeedCDDA						(int16_t* aSamples, uint32_t aFrames)
{
	//TODO
}

