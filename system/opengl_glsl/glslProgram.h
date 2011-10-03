#pragma once

namespace							LibESGL
{
	struct							ProgramPrivate;

	class							Program
	{
		public:
			typedef int32_t			TokenID;

		public:
									Program				(const char* aVertex, const char* aFragment, bool aVertexFile, bool aFragmentFile);
									~Program			();

			void					Use					();
			TokenID					ObtainToken			(const char* aName, bool aFragment);

		private:
			ProgramPrivate*			Private;
	};
}
