#ifndef ES_PROFILE_H
#define ES_PROFILE_H

#include "../../ps3_system.h"

class						Profile
{
	public:
							Profile						()
		{
			Ticks = 0;
		}

		void				Tick						()
		{
			TickTime = Utility::GetTicks();
		}

		void				Tock						()
		{
			Ticks += Utility::GetTicks() - TickTime;
		}

		uint32_t			GetTicks					()
		{
			return Ticks;
		}

		void				Reset						()
		{
			Ticks = 0;
			TickTime = 0;
		}

	protected:
		uint32_t			Ticks;
		uint32_t			TickTime;
};

#endif
