#include "stdafx.h"
#define _IN_OSS
#include "externals.h"

uint8_t SoundBuf[48000 * 4 * 2];
uint32_t SoundBufLen;

// SETUP SOUND
void SetupSound(void)
{
  SoundBufLen = 0;
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
