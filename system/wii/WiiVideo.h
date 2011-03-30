#ifndef WiiVideo_H
#define	WiiVideo_H

class								WiiVideo : public ESVideo
{
	friend class					WiiTexture;

	public:	
									WiiVideo				();
									~WiiVideo				();
	
		Texture*					CreateTexture			(uint32_t aWidth, uint32_t aHeight, bool aStatic = false) {return new WiiTexture(aWidth, aHeight);};
	
		virtual void				SetClip					(Area aClip);
	
		void						Flip					();
		
		void						PlaceTexture			(Texture* aTexture, const Area& aDestination, const Area& aSource, uint32_t aColor);
		void						FillRectangle			(const Area& aArea, uint32_t aColor);
		void						PresentFrame			(Texture* aTexture, const Area& aViewPort, int32_t aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine);
		
	private:
		static void					HandleRetrace			(uint32_t unused);

	private:
		static const int			FIFOSize = 1024 * 256;

		Texture*					FillerTexture;

		void*						FrameBuffer[2];
		uint32_t					CurrentFrameBuffer;
		bool						FirstFrame;
		void*						FIFOBuffer;
		GXRModeObj*					ScreenMode;

		bool						ReadyForCopy;
};

#endif
