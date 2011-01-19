#ifndef SYSTEM__SUMMERFACE_WINDOW_H
#define SYSTEM__SUMMERFACE_WINDOW_H

class	Summerface;

#include	"SummerfaceInputConduit.h"

class													SummerfaceWindow
{
	public:
														SummerfaceWindow				(const Area& aRegion);
		virtual											~SummerfaceWindow				();

		virtual bool									PrepareDraw						();

		virtual bool									Draw							() = 0;
		virtual bool									Input							();

		virtual void									SetInterface					(Summerface* aInterface, const std::string& aName);
		virtual Summerface*								GetInterface					();

		virtual void									SetName							(const std::string& aName);
		virtual std::string								GetName							();

		virtual void									SetInputConduit					(SummerfaceInputConduit* aInputConduit, bool aDelete);
		virtual SummerfaceInputConduit*					GetInputConduit					();

		virtual void									SetNoDelete						();
		virtual bool									GetNoDelete						();

	private:
		static const uint32_t							BorderWidth = 4;

		SummerfaceInputConduit*							InputHandler;
		bool											DeleteHandler;

		bool											NoDelete;

		Summerface*										Interface;
		std::string										Name;
		Area											Region;
		Area											Client;
};


#endif

