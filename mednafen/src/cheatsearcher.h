#pragma once

class													CheatSearcher
{
	private:
		struct											Result
		{
			uint32_t									Address;
			int64_t										Original;
			int64_t										Changed;
		};

		typedef AnchoredListView<SummerfaceItem>		ModeListType;
		typedef smartptr::shared_ptr<ModeListType>		ModeListType_Ptr;	
		typedef SummerfaceItemUser<Result>				ResultListItem;
		typedef AnchoredListView<ResultListItem>		ResultListType;
		typedef smartptr::shared_ptr<ResultListType>	ResultListType_Ptr;	

		static ResultListType_Ptr						ResultList;
		static int										GetResults				(uint32_t aAddress, uint64_t aOriginal, uint64_t aValue, void* aData)
		{
			//The name
			std::stringstream name;
			name << std::hex << aAddress << std::dec << " " << aOriginal << " " << aValue;

			//The value
			Result r = {aAddress, aOriginal, aValue};

			//Add to list
			ResultList->AddItem(boost::make_shared<ResultListItem>(name.str(), "", r));

			return 1;
		}


	public:
		static int					DoSearchFilterMenu		();
		static bool					DoResultList			(uint32_t aBytes, bool aBigEndian);

		static void					Do						();

		static void					Reset					();

	private:
		static int32_t				Mode;
		static int32_t				State;

		static ModeListType_Ptr		SearchFilterList;

		static int64_t				Original;
		static int64_t				Changed;
};

