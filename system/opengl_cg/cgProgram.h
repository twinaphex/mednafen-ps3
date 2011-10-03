#pragma once

#ifndef __CELLOS_LV2__
# include	<Cg/cg.h>
# include	<Cg/cgGL.h>
#endif

namespace							LibESGL
{
	struct							ProgramPrivate;

	class							Program
	{
		public:
			typedef void*			TokenID;

		public:
									Program				(const char* aVertex, const char* aFragment, bool aVertexFile, bool aFragmentFile);
									~Program			();

			static void				Revert				();

			void					Use					();
			TokenID					ObtainToken			(const char* aName, bool aFragment = false);

		private:
			ProgramPrivate*			Private;
	};
}

