#ifndef WiiTexture_H
#define	WiiTexture_H

class								WiiTexture : public Texture
{
	friend class					WiiVideo;

	public:
									WiiTexture				(uint32_t aWidth, uint32_t aHeight);
		virtual						~WiiTexture				();
		
		void						Clear					(uint32_t aColor);
		uint32_t*					GetPixels				();

		uint32_t					GetFlags				() const {return USE_IS_DISCARD;};

		uint32_t					GetRedShift				() const {return 16;};
		uint32_t					GetGreenShift			() const {return 8;};
		uint32_t					GetBlueShift			() const {return 0;};
		uint32_t					GetAlphaShift			() const {return 24;};

	public: //WiiTexture specific helpers, do not call directly
		uint32_t					MultipleOfFour			(uint32_t aNumber) const {return aNumber + ((aNumber % 4) ? (4 - aNumber % 4) : 0);};
		void						Apply					(uint32_t aWidth, uint32_t aHeight);
		
	private:
		uint32_t					AdjustedHeight;
		uint32_t*					Pixels;

		GXTexObj					TextureObject;
};

#endif
