#ifndef SYSTEM__ARCHIVE_H
#define SYSTEM__ARCHIVE_H

class												ArchiveList : public SummerfaceLineList
{
	public:
													ArchiveList						(const Area& aRegion, const std::string& aHeader, const std::string& aFileName);
		virtual										~ArchiveList					();

		uint32_t									GetSelectedSize					();
		void										GetSelectedData					(uint32_t aSize, void* aData);
		std::string									GetSelectedFileName				();
		
		static bool									IsArchive						(const std::string& aFileName);
		
	protected:
		fex_t*										Archive;
		std::string									FileName;
};

#endif
