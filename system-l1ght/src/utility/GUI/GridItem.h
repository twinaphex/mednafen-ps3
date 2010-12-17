#ifndef SYSTEM__GRIDITEM_H
#define	SYSTEM__GRIDITEM_H

class								GridItem : public ListItem
{
	public:	//Inlines
									GridItem							(const std::string& aText, const std::string& aImage);

		virtual void				Draw								(uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected);
		
		virtual uint32_t			GetWidth							();
		virtual uint32_t			GetHeight							();
};
#endif