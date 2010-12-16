#include "ps3_system.h"

#include "ttffont.bin.h"
#include "fixedfont.bin.h"


						Font::Font					(uint32_t aPixelSize, bool aFixed = false)
{
	FT_New_Memory_Face(FontManager::FreeType, (FT_Byte*)(aFixed ? fixedfont_bin : ttffont_bin), sizeof(ttffont_bin), 0, &FontFace);
	FT_Set_Pixel_Sizes(FontFace, 0, aPixelSize); 

	Width = FontFace->size->metrics.max_advance / 64;
	Height = FontFace->size->metrics.height / 64;
}

						Font::~Font					()
{
	for(std::map<uint32_t, FontCharacter*>::iterator i = Cache.begin(); i != Cache.end(); i ++)
	{
		delete i->second;
	}
		
	FT_Done_Face(FontFace);
}

void					Font::PutString					(const char* aString, uint32_t aX, uint32_t aY, uint32_t aColor)
{
	for(int i = 0; i != strlen(aString); i ++)
	{
		if(aString[i] >= 32)
		{
			FontCharacter* chara = CacheCharacter(aString[i]);
			
			if(chara->CharTexture)
			{
				PS3Video::PlaceTexture(chara->CharTexture, aX + chara->BaseX, aY + Height - chara->BaseY, chara->Width, chara->Height, aColor);
			}
			
			aX += chara->Advance;
		}
	}
}

uint32_t				Font::GetWidth				()
{
	return Width;
}

uint32_t				Font::GetHeight				()					
{
	return Height;
}

FontCharacter*			Font::CacheCharacter		(uint32_t aCharacter)
{
	if(Cache[aCharacter] == 0)
	{
		FontCharacter* character = new FontCharacter();

		FT_UInt glyph_index = FT_Get_Char_Index(FontFace, aCharacter);
		FT_Load_Glyph(FontFace, glyph_index, FT_LOAD_RENDER);

		character->Advance = FontFace->glyph->advance.x >> 6;
		character->Width = FontFace->glyph->bitmap.width;
		character->Height = FontFace->glyph->bitmap.rows;
		character->BaseX = FontFace->glyph->bitmap_left;
		character->BaseY = FontFace->glyph->bitmap_top;
		
		if(character->Width != 0 && character->Height != 0)
		{
			character->CharTexture = new Texture(FontFace->glyph->bitmap.width, FontFace->glyph->bitmap.rows);
		
			uint32_t* pixels = character->CharTexture->GetPixels();
	
			for(int y = 0; y != FontFace->glyph->bitmap.rows; y ++)
			{
				uint8_t* row = &FontFace->glyph->bitmap.buffer[y * FontFace->glyph->bitmap.pitch];
			
				for(int x = 0; x != FontFace->glyph->bitmap.width; x ++)
				{
					pixels[y * character->Width + x] = 0xFFFFFF | (row[x] << 24);
				}
			}
		}
		else
		{
			character->CharTexture = 0;
		}

		Cache[aCharacter] = character;
	}
	
	return Cache[aCharacter];
}

Font*					FontManager::GetBigFont		()
{
	if(!FontsOpen)
	{
		throw "FontManager: Attempt to use before initialization";
	}
	
	return BigFont;
}

Font*					FontManager::GetSmallFont	()
{
	if(!FontsOpen)
	{
		throw "FontManager: Attempt to use before initialization";
	}

	return SmallFont;
}

Font*					FontManager::GetTinyFont	()
{
	if(!FontsOpen)
	{
		throw "FontManager: Attempt to use before initialization";
	}

	return TinyFont;
}

Font*					FontManager::GetFixedFont	()
{
	if(!FontsOpen)
	{
		throw "FontManager: Attempt to use before initialization";
	}

	return FixedFont;
}

void					FontManager::InitFonts		()
{
	if(!FontsOpen)
	{
		FT_Init_FreeType(&FreeType);
	
		BigFont = new Font(PS3Video::GetScreenHeight() / 25);
		SmallFont = new Font(PS3Video::GetScreenHeight() / 40);
		TinyFont = new Font(12);
		FixedFont = new Font(PS3Video::GetScreenHeight() / 36, true);
	}
	
	FontsOpen = true;
}

void					FontManager::QuitFonts		()
{
	if(FontsOpen)
	{
		delete BigFont;
		delete SmallFont;
		delete TinyFont;
		delete FixedFont;
		
		FT_Done_FreeType(FreeType);
	}
	
	FontsOpen = false;
}

FT_Library				FontManager::FreeType = 0;

bool					FontManager::FontsOpen = false;
Font*					FontManager::BigFont = 0;
Font*					FontManager::SmallFont = 0;
Font*					FontManager::TinyFont = 0;
Font*					FontManager::FixedFont = 0;

