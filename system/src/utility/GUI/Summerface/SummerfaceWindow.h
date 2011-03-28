#ifndef SYSTEM__SUMMERFACE_WINDOW_H
#define SYSTEM__SUMMERFACE_WINDOW_H

class	Summerface;

#include	"SummerfaceInputConduit.h"

class													SummerfaceWindow
{
	public:
														SummerfaceWindow				(const Area& aRegion);
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

		virtual void									SetInputConduit					(SummerfaceInputConduit_Ptr aInputConduit) {InputHandler = aInputConduit;}
		virtual SummerfaceInputConduit_Ptr				GetInputConduit					() const {return InputHandler;};

	private:
		static const uint32_t							BorderWidth = 4;

		Summerface_WeakPtr								Interface;
		SummerfaceInputConduit_Ptr						InputHandler;

		std::string										Name;
		std::string										Header;
		Area											Region;
		Area											Client;
};


#endif

