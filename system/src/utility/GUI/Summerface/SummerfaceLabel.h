#ifndef SYSTEM__SUMMERFACE_LABEL_H
#define SYSTEM__SUMMERFACE_LABEL_H

#include "SummerfaceWindow.h"

class													SummerfaceLabel : public SummerfaceWindow
{
	public:
														SummerfaceLabel					(const Area& aRegion, const std::string& aMessage);
		virtual											~SummerfaceLabel				();

		virtual bool									Draw							();

		virtual std::string								GetMessage						();
		virtual void									SetMessage						(const std::string& aMessage);
		

	private:
		std::string										Message;
};

#endif

