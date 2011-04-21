#ifndef L1GHT_SECRET_H
#define L1GHT_SECRET_H

class				L1ghtSecret : public SummerfaceList
{
	public:
					L1ghtSecret					();
					~L1ghtSecret				();

		bool		Input						();
		void		Do							();

	protected:
		Summerface*	UI;
};

#endif

