#include <ps3_system.h>

void										ImageManager::Purge									()
{
	for(std::map<std::string, Texture*>::iterator i = Images.begin(); i != Images.end(); i ++)
	{
		delete i->second;
	}
	
	Images.clear();
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
	
Texture*									ImageManager::LoadImage								(const std::string& aName, const std::string& aPath)
{
	if(Images[aName] == 0)
	{
		char header[8];
		
		FILE *fp = fopen(aPath.c_str(), "rb");
		if (!fp)
			Abort("[read_png_file] File could not be opened for reading");
		fread(header, 1, 8, fp);
			if (png_sig_cmp((png_bytep)header, 0, 8))
				Abort("[read_png_file] File is not recognized as a PNG file");
	
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr)
			Abort("[read_png_file] png_create_read_struct failed");
	
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
			Abort("[read_png_file] png_create_info_struct failed");
	
		if (setjmp(png_jmpbuf(png_ptr)))
			Abort("[read_png_file] Error during init_io");
	
		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, 8);
	
		if (setjmp(png_jmpbuf(png_ptr)))
			Abort("[read_png_file] Error during read_image");

#ifdef L1GHT	
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_SWAP_ALPHA, 0);
#else
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_BGR, 0);
#endif
	
		fclose(fp);

		png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);
		png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
		png_uint_32 height = png_get_image_height(png_ptr, info_ptr);		

		Texture* output = es_video->CreateTexture(width, height);
		output->SetFilter(true);

		for(int i = 0; i != height; i ++)
		{
			memcpy(output->GetPixels() + (output->GetWidth() * i), row_pointers[i], width * 4);
		}
		
		png_destroy_info_struct(png_ptr, &info_ptr);
		png_destroy_read_struct(&png_ptr, 0, 0);
		
		Images[aName] = output;
	}
	
	return Images[aName];
}

Texture*									ImageManager::GetImage								(const std::string& aName)
{
	return Images[aName];
}


std::map<std::string, Texture*>				ImageManager::Images;
