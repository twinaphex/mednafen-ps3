#include <es_system.h>

											ImageManager::PNGFile::PNGFile						(const std::string& aPath)
{
	char header[8];
		
	FILE *fp = fopen(aPath.c_str(), "rb");
	if (!fp) Abort("[read_png_file] File could not be opened for reading");

	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_bytep)header, 0, 8)) Abort("[read_png_file] File is not recognized as a PNG file");
	
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) Abort("[read_png_file] png_create_read_struct failed");
	
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) Abort("[read_png_file] png_create_info_struct failed");
	
	if (setjmp(png_jmpbuf(png_ptr))) Abort("[read_png_file] Error during init_io");
	
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);
	
	if (setjmp(png_jmpbuf(png_ptr))) Abort("[read_png_file] Error during read_image");
	
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_SWAP_ALPHA, 0);
	
	fclose(fp);

	row_pointers = png_get_rows(png_ptr, info_ptr);
	Width = png_get_image_width(png_ptr, info_ptr);
	Height = png_get_image_height(png_ptr, info_ptr);		
}

											ImageManager::PNGFile::~PNGFile						()
{
	png_destroy_info_struct(png_ptr, &info_ptr);
	png_destroy_read_struct(&png_ptr, 0, 0);
}

void										ImageManager::PNGFile::CopyToTexture				(Texture* aTexture)
{
	aTexture->Clear(0);

	for(int i = 0; i != Height; i ++)
	{
		uint32_t* dest = aTexture->GetPixels() + (aTexture->GetPitch() * i);
		uint8_t* source = row_pointers[i];

		for(int j = 0; j != Width; j ++)
		{
			uint32_t a = PNG_COLOR_TYPE_RGB_ALPHA ? *source++ : 0xFF;
			uint32_t r = *source ++;
			uint32_t g = *source ++;
			uint32_t b = *source ++;
			*dest++ = aTexture->ConvertPixel(r, g, b, a);
		}
	}
}

void										ImageManager::Purge									()
{
	for(std::map<std::string, Texture*>::iterator i = Images.begin(); i != Images.end(); i ++)
	{
		delete i->second;
	}

	for(std::map<std::string, PNGFile*>::iterator i = Files.begin(); i != Files.end(); i ++)
	{
		delete i->second;
	}
	
	Images.clear();
	Files.clear();
}

void										ImageManager::LoadDirectory							(const std::string& aPath)
{
	std::vector<std::string> items;

	if(Utility::ListDirectory(aPath, items))
	{
		for(int i = 0 ; i != items.size(); i ++)
		{
			if(items[i].find(".png") != std::string::npos)
			{
				LoadImage(items[i].substr(0, items[i].length() - 4), aPath + items[i]);
			}
		}
	}
	else
	{
		printf("ImageManager::LoadDirectory: Path '%s' not found\n", aPath.c_str());
	}
}

void										ImageManager::CreateScratch							()
{
	if(!Images["SCRATCH%0"])
	{
		char buffer[32];
		for(int i =0; i != 20; i ++)
		{
			Texture* tex = es_video->CreateTexture(512, 512, false);
			tex->SetFilter(1);

			snprintf(buffer, 32, "SCRATCH%%%d", i);
			Images[buffer] = tex;
		}
	}
}
	
Texture*									ImageManager::LoadImage								(const std::string& aName, const std::string& aPath)
{
	if(Images[aName] == 0)
	{
		PNGFile ping(aPath);

		Texture* output = es_video->CreateTexture(ping.Width, ping.Height, true);
		output->SetFilter(true);

		ping.CopyToTexture(output);
		
		Images[aName] = output;
	}
	
	return Images[aName];
}

Texture*									ImageManager::GetImage								(const std::string& aName)
{
	return Images[aName];
}

void										ImageManager::FillScratch							(uint32_t aScratchIndex, const std::string& aFileName)
{
	if(aScratchIndex >= 20)
	{
		throw ESException("FillScratch: Index out of range [Index: %d]", aScratchIndex);
	}
	
	char buffer[256];
	snprintf(buffer, 256, "SCRATCH%%%d", aScratchIndex);
	Texture* scratch = GetImage(buffer);
	
	if(!scratch)
	{
		throw ESException("FillScratch: Scratch asset not available [Index: %d]", aScratchIndex);
	}
	
	if(Files[aFileName] == 0)
	{
		//Load it
		PNGFile* file = new PNGFile(aFileName);
		Files[aFileName] = file;
	}
	
	Files[aFileName]->CopyToTexture(scratch);
}


std::map<std::string, Texture*>				ImageManager::Images;
std::map<std::string, ImageManager::PNGFile*>ImageManager::Files;