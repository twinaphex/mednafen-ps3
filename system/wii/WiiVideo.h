#ifndef WiiVideo_H
#define	WiiVideo_H

class								WiiVideo;

class								WiiTexture : public Texture
{
	friend class					WiiVideo;

	public:
									WiiTexture				(uint32_t aWidth, uint32_t aHeight, bool aStatic);
		virtual						~WiiTexture				();
		
		void						Clear					(uint32_t aColor);
		uint32_t*					GetPixels				();
		
		uint32_t					GetWidth				() {return Width;};
		uint32_t					GetHeight				() {return Height;};
		uint32_t					GetPitch				() {return Pitch;};

		void						SetFilter				(uint32_t aOn) {Filter = aOn ? 1 : 0;}

		uint32_t					GetRedShift				() const {return 0;};
		uint32_t					GetGreenShift			() const {return 8;};
		uint32_t					GetBlueShift			() const {return 16;};
		uint32_t					GetAlphaShift			() const {return 24;};

	private:
		void						Apply					();
		
	private:
		GXTexObj					TextureObject;

		bool						Static;
		bool						Locked;

		uint32_t					Width;
		uint32_t					Height;
		uint32_t					Pitch;
		uint32_t					AdjustedHeight;
		
		uint32_t					Filter;

		uint32_t*					Pixels;
		bool						Valid;
};

class								WiiVideo : public ESVideo
{
	friend class					WiiTexture;

	public:	
									WiiVideo				();
									~WiiVideo				();
	
		Texture*					CreateTexture			(uint32_t aWidth, uint32_t aHeight, bool aStatic = false) {return new WiiTexture(aWidth, aHeight, true);};//HACK
	
		bool						IsWideScreen			() {return true;};

		virtual void				SetClip					(Area aClip);
	
		void						Flip					();
		
		void						PlaceTexture			(Texture* aTexture, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, uint32_t aColor = 0xFFFFFFFF, Area* aArea = 0);
		void						FillRectangle			(Area aArea, uint32_t aColor);
		void						PresentFrame			(Texture* aTexture, Area aViewPort, bool aAspectOverride, int32_t aUnderscan, const Area& aUnderscanFine);
		
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
