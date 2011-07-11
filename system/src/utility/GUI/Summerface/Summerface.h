#pragma once

#include "SummerfaceWindow.h"

class													Summerface : public Menu, public smartptr::enable_shared_from_this<Summerface>
{
	typedef std::set<SummerfaceInputConduit_Ptr>		ConduitSet;

	public: //Do not call!
														Summerface						() {};

	public:
		static Summerface_Ptr							Create							(); //External
		static Summerface_Ptr							Create							(const std::string& aName, SummerfaceWindow_Ptr aWindow); //External

		virtual											~Summerface						() {};

		virtual bool									Draw							();
		virtual bool									Input							();

		void											AddWindow						(const std::string& aName, SummerfaceWindow_Ptr aWindow);
		void											RemoveWindow					(const std::string& aName);
		SummerfaceWindow_Ptr							GetWindow						(const std::string& aName);
		void											SetActiveWindow					(const std::string& aName);

		void											AttachConduit					(SummerfaceInputConduit_Ptr aConduit) {Handlers.insert(aConduit);}
		void											DetachConduit					(SummerfaceInputConduit_Ptr aConduit) {Handlers.erase(aConduit);}

		static void										SetDrawBackground				(bool (*aCallback)()) {BackgroundCallback = aCallback;};

	private:
		ConduitSet										Handlers;

		std::map<std::string, SummerfaceWindow_Ptr>		Windows;
		std::string										ActiveWindow;

		static bool										(*BackgroundCallback)			();
};

