#pragma once

class									ESAudio
{
	public:	
		static void						Initialize				();
		static void						Shutdown				();

		static void						AddSamples				(const uint32_t* aSamples, uint32_t aCount);
		static volatile int32_t			GetBufferAmount			();
		static volatile int32_t			GetBufferFree			();
		static void						SetSpeed				(uint32_t aSpeed);
};

