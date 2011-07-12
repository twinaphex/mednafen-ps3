#ifndef SYSTEM__SUMMERFACE_WINDOW_H
#define SYSTEM__SUMMERFACE_WINDOW_H

class	Summerface;

class													SummerfaceCancelable
{
	public:
		bool											WasCanceled						() {return Canceled;}
		void											SetCanceled						(bool aCanceled) {Canceled = aCanceled;}

	protected:
		bool											Canceled;
};

class													SummerfaceWindow
{
	public:
														SummerfaceWindow				(const Area& aRegion, bool aBorder = true);
		virtual											~SummerfaceWindow				() {};

		virtual bool									PrepareDraw						(); //External

		virtual bool									Draw							() = 0; //Pure virtual
		virtual bool									Input							(); //External

		virtual void									SetInterface					(Summerface_Ptr aInterface, const std::string& aName); //External
		virtual Summerface_Ptr							GetInterface					(); //External

		virtual void									SetName							(const std::string& aName) {Name = aName;}
		virtual std::string								GetName							() const {return Name;}

		virtual void									SetHeader						(const std::string& aHeader, ...); //External
		virtual std::string								GetHeader						() const {return Header;};

		virtual bool									SetBorder						(bool aEnable) {UseBorder = aEnable;}

	private:
		static const uint32_t							BorderWidth = 4;

		Summerface_WeakPtr								Interface;

		std::string										Name;
		std::string										Header;
		Area											Region;
		Area											Client;

		bool											UseBorder;
};


#endif

