#ifndef FONT_H_
#define FONT_H_

class										FontCharacter
{
	public:
		Texture*							CharTexture;
		uint32_t							Width;
		uint32_t							Height;
		uint32_t							Advance;
		uint32_t							BaseX;
		uint32_t							BaseY;
};

class										Font
{
	public:
											Font						(uint32_t aPixelSize, bool aFixed);
											~Font						();
								
		void								PutString					(const std::string& aString, uint32_t aX, uint32_t aY, uint32_t aColor);
		Area								MeasureString				(const std::string& aString);

		uint32_t							GetWidth					()					{return Width;};
		uint32_t							GetHeight					()					{return Height;};
		
	protected:
		FontCharacter*						CacheCharacter				(uint32_t aCharacter);
	
		FT_Face								FontFace;
		uint32_t							Width;
		uint32_t							Height;
		
		std::map<uint32_t, FontCharacter*>	Cache;
};

class										FontManager
{
	friend class							PS3Video;
	friend class							Font;

	public:
		static Font*						GetBigFont					();
		static Font*						GetSmallFont				();
		static Font*						GetTinyFont					();
		static Font*						GetFixedFont				();
		
	protected:
		static void							InitFonts					();
		static void							QuitFonts					();
	
		static FT_Library					FreeType;	
	
		static bool							FontsOpen;
		static Font*						TinyFont;		
		static Font*						SmallFont;
		static Font*						FixedFont;
		static Font*						BigFont;
};

#endif
