#ifndef WiiTexture_H
#define	WiiTexture_H

class								WiiTexture : public Texture
{
	friend class					WiiVideo;

	public:
									WiiTexture				(uint32_t aWidth, uint32_t aHeight, bool aStatic);
		virtual						~WiiTexture				();
		
		void						Clear					(uint32_t aColor);
		uint32_t*					GetPixels				();
		void						Invalidate				() {Valid = false;};		

		uint32_t					GetWidth				() {return Width;};
		uint32_t					GetHeight				() {return Height;};
		uint32_t					GetPitch				() {return Pitch;};

		void						SetFilter				(uint32_t aOn) {Filter = aOn ? 1 : 0;}

		uint32_t					GetFlags				() const {return USE_IS_DISCARD;};
		uint32_t					GetRedShift				() const {return 16;};
		uint32_t					GetGreenShift			() const {return 8;};
		uint32_t					GetBlueShift			() const {return 0;};
		uint32_t					GetAlphaShift			() const {return 24;};

	public: //WiiTexture specific helpers, do not call directly
		uint32_t					MultipleOfFour			(uint32_t aNumber) const {return aNumber + ((aNumber % 4) ? (4 - aNumber % 4) : 0);};
		void						Apply					(uint32_t aWidth, uint32_t aHeight);
		
	private:
		bool						Static;
		bool						Locked;

		uint32_t					Width;
		uint32_t					Height;
		uint32_t					Pitch;
		uint32_t					AdjustedHeight;
		
		uint32_t					Filter;

		uint32_t*					Pixels;
		bool						Valid;

		GXTexObj					TextureObject;
};

#endif
