#ifndef SYSTEM__GRIDITEM_H
#define	SYSTEM__GRIDITEM_H

class								GridItem : public ListItem
{
	public:	//Inlines
									GridItem							(const std::string& aText, const std::string& aImage);
		virtual						~GridItem							(){};

		virtual void				Draw								(uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected);
		
		virtual uint32_t			GetWidth							();
		virtual uint32_t			GetHeight							();

		virtual void				SetImageArea						(uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight);

	protected:
		Area						ImageArea;
};
#endif

