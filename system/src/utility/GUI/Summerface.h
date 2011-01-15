#ifndef SYSTEM__SUMMERFACE_H
#define SYSTEM__SUMMERFACE_H

class	Summerface;

class													SummerfaceWindow
{
	public:
														SummerfaceWindow				(const Area& aRegion);
		virtual											~SummerfaceWindow				();

		bool											PrepareDraw						();

		virtual bool									Draw							() = 0;
		virtual bool									Input							() = 0;

		Summerface*										GetInterface					();
		std::string										GetName							();
		void											SetInterface					(Summerface* aInterface, const std::string& aName);

	protected:
		static const uint32_t							BorderWidth = 4;

		Summerface*										Interface;
		std::string										Name;
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

	public:
		std::map<std::string, std::string>				Properties;

	protected:
		std::string										Text;
		std::string										Image;
};

class													SummerfaceInputConduit
{
	public:
		virtual bool									HandleInput						(const std::string& aWindow, SummerfaceItem* aItem) = 0;
};

class													SummerfaceStaticConduit	: public SummerfaceInputConduit
{
	public:
														SummerfaceStaticConduit			(bool (*aCallback)(void*, const std::string&, SummerfaceItem*), void* aUserData);

		virtual bool									HandleInput						(const std::string& aWindow, SummerfaceItem* aItem);

	protected:
		bool											(*Callback)						(void*, const std::string&, SummerfaceItem*);
		void*											UserData;
};

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

		void											SetInputConduit					(SummerfaceInputConduit* aInputConduit, bool aDelete);
		SummerfaceInputConduit*							GetInputConduit					();

		void											SetFont							(Font* aFont);

	protected:
		std::vector<SummerfaceItem*>					Items;
		uint32_t										SelectedIndex;
		bool											Canceled;

		Font*											LabelFont;

		SummerfaceInputConduit*							InputHandler;
		bool											DeleteInputHandler;
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


class													Summerface : public Menu
{
	public:
														Summerface						();
														Summerface						(const std::string& aName, SummerfaceWindow* aWindow);

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

