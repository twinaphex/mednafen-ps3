#include "stdafx.h"
#define _IN_OSS
#include "externals.h"

uint8_t SoundBuf[96000];
uint32_t SoundBufLen = 0;

// SETUP SOUND
void SetupSound(void)
{
}

// REMOVE SOUND
void RemoveSound(void)
{
}

// GET BYTES BUFFERED
unsigned long SoundGetBytesBuffered(void)
{
  return SoundBufLen;
}

// FEED SOUND DATA
void SoundFeedStreamData(unsigned char* pSound,long lBytes)
{
	memcpy(&SoundBuf[SoundBufLen], pSound, lBytes);
	SoundBufLen += lBytes;
}
