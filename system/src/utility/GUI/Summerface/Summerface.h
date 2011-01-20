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
		void											RemoveWindow					(const std::string& aName, bool aDelete);
		SummerfaceWindow*								GetWindow						(const std::string& aName);

		void											SetActiveWindow					(const std::string& aName);

		static void										SetDrawBackground				(void (*aCallback)());
		
	private:
		std::map<std::string, SummerfaceWindow*>		Windows;
		std::string										ActiveWindow;

		static void										(*BackgroundCallback)			();
};


#endif

