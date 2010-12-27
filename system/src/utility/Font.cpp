#include "ps3_system.h"

						Font::Font					(uint32_t aPixelSize, bool aFixed = false)
{
	if(0 != FT_New_Face(FontManager::FreeType, aFixed ? es_paths->Build("assets/font/fixed.ttf").c_str() : es_paths->Build("assets/font/prop.ttf").c_str(), 0, &FontFace))
	{
		Abort("Font::Font: FT_New_Face failed");
	}
	
	if(0 != FT_Set_Pixel_Sizes(FontFace, 0, aPixelSize))
	{
		Abort("Font::Font: FT_Set_Pixel_Sizes failed");
	}

	Width = FontFace->size->metrics.max_advance / 64;
	Height = FontFace->size->metrics.height / 64;
}

						Font::~Font					()
{
	for(std::map<uint32_t, FontCharacter*>::iterator i = Cache.begin(); i != Cache.end(); i ++)
	{
		delete i->second->CharTexture;
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
			
			if(chara && chara->CharTexture)
			{
				es_video->PlaceTexture(chara->CharTexture, aX + chara->BaseX, aY + Height - chara->BaseY, chara->Width, chara->Height, aColor);
			}
			
			if(chara)
			{
				aX += chara->Advance;
			}
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

		if(0 != FT_Load_Glyph(FontFace, glyph_index, FT_LOAD_RENDER))
		{
			Abort("Font::CacheCharacter: FT_Load_Glyph failed");
		}

		character->Advance = FontFace->glyph->advance.x >> 6;
		character->Width = FontFace->glyph->bitmap.width;
		character->Height = FontFace->glyph->bitmap.rows;
		character->BaseX = FontFace->glyph->bitmap_left;
		character->BaseY = FontFace->glyph->bitmap_top;
		
		if(character->Width != 0 && character->Height != 0)
		{
			character->CharTexture = es_video->CreateTexture(FontFace->glyph->bitmap.width, FontFace->glyph->bitmap.rows);
		
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
		Abort("FontManager: Attempt to use before initialization");
	}
	
	return BigFont;
}

Font*					FontManager::GetSmallFont	()
{
	if(!FontsOpen)
	{
		Abort("FontManager: Attempt to use before initialization");
	}

	return SmallFont;
}

Font*					FontManager::GetFixedFont	()
{
	if(!FontsOpen)
	{
		Abort("FontManager: Attempt to use before initialization");
	}

	return FixedFont;
}

void					FontManager::InitFonts		()
{
	if(!FontsOpen)
	{
		if(0 != FT_Init_FreeType(&FreeType))
		{
			Abort("FontManager::Init: Failed to initialize freetype");
		}
	
		BigFont = new Font(es_video->GetScreenHeight() / 25);
		SmallFont = new Font(es_video->GetScreenHeight() / 40);
		FixedFont = new Font(es_video->GetScreenHeight() / 36, true);
	}
	
	FontsOpen = true;
}

void					FontManager::QuitFonts		()
{
	if(FontsOpen)
	{
		delete BigFont;
		delete SmallFont;
		delete FixedFont;
		
		FT_Done_FreeType(FreeType);
	}
	
	FontsOpen = false;
}

FT_Library				FontManager::FreeType = 0;

bool					FontManager::FontsOpen = false;
Font*					FontManager::BigFont = 0;
Font*					FontManager::SmallFont = 0;
Font*					FontManager::FixedFont = 0;

