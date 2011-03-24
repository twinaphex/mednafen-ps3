#ifndef WiiAudio_H
#define	WiiAudio_H

class								WiiAudio : public ESAudio
{
	public:	
									WiiAudio				();
									~WiiAudio				();

		void						AddSamples				(uint32_t* aSamples, uint32_t aCount);
		volatile int32_t			GetBufferFree			() {return BufferSize - (WriteCount - ReadCount);};
		volatile int32_t			GetBufferAmount			() {return WriteCount - ReadCount;};

	protected:
		void						GetSamples				(uint32_t* aSamples, uint32_t aCount);
		void						InitDMA					();
		static void					ProcessAudioCallback	();

		static const int			BufferSize = 8192;
		static const int			BufferMask = 0x1FFF;
		static const int			SegmentSize = 800;

		uint32_t*					Buffer[2];
		uint32_t					BufferIndex;

		mutex_t						BufferMutex;
		uint32_t 					RingBuffer[BufferSize];
		volatile int32_t			ReadCount;
		volatile int32_t			WriteCount;
};

#endif
