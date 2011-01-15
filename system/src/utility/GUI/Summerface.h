#ifndef SYSTEM__SUMMERFACE_H
#define SYSTEM__SUMMERFACE_H

class	Summerface;

class													SummerfaceWindow
{
	public:
														SummerfaceWindow				(Summerface* aInterface, const Area& aRegion);
		virtual											~SummerfaceWindow				();

		bool											PrepareDraw						();

		virtual bool									Draw							() = 0;
		virtual bool									Input							() = 0;

	protected:
		static const uint32_t							BorderWidth = 4;

		Summerface*										Interface;
		Area											Region;
		Area											Client;
};

class													SummerfaceItem
{
	public:
														SummerfaceItem					(const std::string& aText, const std::string& aImage);
		virtual											~SummerfaceItem					();

		virtual void									SetText							(const std::string& aText);
		virtual void									SetImage						(const std::string& aImage);

		virtual std::string								GetText							();
		virtual std::string								GetImage						();

	protected:
		std::string										Text;
		std::string										Image;
};

class													SummerfaceGrid : public SummerfaceWindow
{
	public:
														SummerfaceGrid					(Summerface* aInterface, const Area& aRegion, uint32_t aWidth, uint32_t aHeight);
		virtual											~SummerfaceGrid					();


		virtual bool									DrawItem						(SummerfaceItem* aItem, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected);
		virtual bool									Draw							();
		virtual bool									Input							();

		uint32_t										GetItemCount					();
		void											SetSelection					(uint32_t aIndex);

		bool											WasCanceled						();


	protected:
		std::vector<SummerfaceItem*>					Items;

		uint32_t										Width;
		uint32_t										Height;

		bool											Canceled;

		int32_t											XSelection;
		int32_t											YSelection;
		int32_t											SelectedIndex;
};

class													Summerface : public Menu
{
	public:
														Summerface						();
		virtual											~Summerface						();

		virtual bool									Input							();
		virtual bool									Draw							();

		void											AddWindow						(const std::string& aName, SummerfaceWindow* aWindow);
		void											SetActiveWindow					(const std::string& aName);
		
	protected:
		std::map<std::string, SummerfaceWindow*>		Windows;
		std::string										ActiveWindow;
};


#endif

