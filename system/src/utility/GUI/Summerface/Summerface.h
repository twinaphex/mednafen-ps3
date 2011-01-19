#ifndef SYSTEM__SUMMERFACE_H
#define SYSTEM__SUMMERFACE_H

#include "SummerfaceWindow.h"

class													Summerface : public Menu
{
	public:
														Summerface						();
														Summerface						(const std::string& aName, SummerfaceWindow* aWindow);

		virtual											~Summerface						();

		virtual bool									Draw							();
		virtual bool									Input							();

		void											AddWindow						(const std::string& aName, SummerfaceWindow* aWindow);
		SummerfaceWindow*								GetWindow						(const std::string& aName);

		void											SetActiveWindow					(const std::string& aName);
		
	protected:
		std::map<std::string, SummerfaceWindow*>		Windows;
		std::string										ActiveWindow;
};


#endif
