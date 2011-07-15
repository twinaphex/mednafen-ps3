#pragma once

class								CheatSearcher
{
	private:
		struct						Result
		{
			uint32_t				Address;
			int64_t					Original;
			int64_t					Changed;
		};

		typedef std::vector<Result>	ResultList;
		static ResultList			Results;
		static int					GetResults				(uint32_t aAddress, uint64_t aOriginal, uint64_t aValue, void* aData)
		{
			Result r = {aAddress, aOriginal, aValue};
			Results.push_back(r);
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

		static Summerface_Ptr		SearchFilterMenu;

		static int64_t				Original;
		static int64_t				Changed;
};

