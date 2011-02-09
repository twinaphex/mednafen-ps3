#ifndef ESAUDIO_H
#define	ESAUDIO_H

class								ESAudio
{
	public:	
		virtual						~ESAudio				(){};
	
		virtual void				AddSamples				(uint32_t* aSamples, uint32_t aCount) = 0;
		virtual volatile int32_t	GetBufferAmount			() = 0;
		virtual volatile int32_t	GetBufferFree			() = 0;
};

#endif
