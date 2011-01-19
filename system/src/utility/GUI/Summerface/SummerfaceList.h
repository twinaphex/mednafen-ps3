#ifndef SYSTEM__SUMMERFACE_LIST_H
#define SYSTEM__SUMMERFACE_LIST_H

class													SummerfaceList : public SummerfaceWindow
{
	public:
														SummerfaceList					(const Area& aRegion);
		virtual											~SummerfaceList					();

		virtual void									AddItem							(SummerfaceItem* aItem);

		SummerfaceItem*									GetSelected						();

		uint32_t										GetItemCount					();
		void											SetSelection					(uint32_t aIndex);

		bool											WasCanceled						();

		void											SetFont							(Font* aFont);

	protected:
		std::vector<SummerfaceItem*>					Items;
		uint32_t										SelectedIndex;
		bool											Canceled;

		Font*											LabelFont;
};

class													SummerfaceGrid : public SummerfaceList
{
	public:
														SummerfaceGrid					(const Area& aRegion, uint32_t aWidth, uint32_t aHeight, bool aHeader = true, bool aLabels = false);
		virtual											~SummerfaceGrid					();

		virtual bool									DrawItem						(SummerfaceItem* aItem, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected);
		virtual bool									Draw							();
		virtual bool									Input							();

		void											SetDrawMode						(bool aHeader, bool aLabels);


	protected:
		uint32_t										Width;
		uint32_t										Height;

		bool											DrawHeader;
		bool											DrawLabels;
};

class													SummerfaceLineList : public SummerfaceList
{
	public:
														SummerfaceLineList				(const Area& aRegion);
		virtual											~SummerfaceLineList				();

		virtual bool									DrawItem						(SummerfaceItem* aItem, uint32_t aX, uint32_t aY, bool aSelected);
		virtual bool									Draw							();
		virtual bool									Input							();

	protected:
		uint32_t										LinesDrawn;
};


#endif

