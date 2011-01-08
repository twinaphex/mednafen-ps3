#ifndef SYSTEM__INPUTLISTITEM_H
#define SYSTEM__INPUTLISTITEM_H

class													InputListItem : public ListItem
{
	public:
														InputListItem					(const std::string& aText, uint32_t aInput);
	
	protected:
		uint32_t										Input;
};

#endif
