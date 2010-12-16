#ifndef SYSTEM__ICONGRID_H
#define SYSTEM__ICONGRID_H

class								GridItem : public ListItem
{
	public:	//Inlines
									GridItem							(const std::string& aText, const std::string& aImage) : ListItem(aText, FontManager::GetSmallFont(), aImage)
		{
		
		}

		virtual void				Draw								(uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected)
		{
			if(ImageManager::GetImage(LabelImage))
			{
				PS3Video::PlaceTexture(ImageManager::GetImage(LabelImage), aX, aY, aWidth, aHeight, 0xFFFFFFFF);
			}
			
			if(aSelected)
			{
				PS3Video::FillRectangle(Area(aX, aY, aWidth, aHeight), 0x40404040);
			}
		}
		
		virtual uint32_t			GetWidth							()
		{
			if(ImageManager::GetImage(LabelImage))
			{
				return ImageManager::GetImage(LabelImage)->GetWidth();
			}
			
			return 0;
		}

		virtual uint32_t			GetHeight							()
		{
			if(ImageManager::GetImage(LabelImage))
			{
				return ImageManager::GetImage(LabelImage)->GetHeight();
			}
			
			return 0;
		}
};

class								WinterfaceIconGrid : public Winterface
{
	public:
									WinterfaceIconGrid						(const std::string& aHeader, uint32_t aWidth, uint32_t aHeight, bool aKillItems);
		bool						DrawLeft								();

	public:	//Inlines
		virtual						~WinterfaceIconGrid						()
		{
			if(KillItems)
			{
				for(std::vector<GridItem*>::iterator iter = Items.begin(); iter != Items.end(); iter ++)
				{
					delete (*iter);
				}
			}
		}
	
		bool						Input									()
		{
			XSelection += PS3Input::ButtonDown(0, PS3_BUTTON_RIGHT) ? 1 : 0;
			XSelection -= PS3Input::ButtonDown(0, PS3_BUTTON_LEFT) ? 1 : 0;
			XSelection = Utility::Clamp(XSelection, 0, (int32_t)Width - 1);

			YSelection += PS3Input::ButtonDown(0, PS3_BUTTON_DOWN) ? 1 : 0;
			YSelection -= PS3Input::ButtonDown(0, PS3_BUTTON_UP) ? 1 : 0;
			YSelection = Utility::Clamp(YSelection, 0, (int32_t)Height - 1);

			if(Items[YSelection * Width + XSelection]->Input())
			{
				return true;
			}

			return Winterface::Input();
		}
	
	protected:
		std::vector<GridItem*>		Items;
		
		uint32_t					Width;
		uint32_t					Height;
		int32_t						XSelection;
		int32_t						YSelection;
};

#endif
