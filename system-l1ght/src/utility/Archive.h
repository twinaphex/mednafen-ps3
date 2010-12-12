#ifndef SYSTEM__ARCHIVE_H
#define SYSTEM__ARCHIVE_H

class												ArchiveList : public WinterfaceList
{
	public:
													ArchiveList						(std::string aHeader, std::string aFileName, MenuHook* aInputHook = 0);
		virtual										~ArchiveList					();

		uint32_t									ItemCount						();

		uint32_t									GetSelectedSize					();
		void										GetSelectedData					(uint32_t aSize, void* aData);
		std::string									GetSelectedFileName				();
		
		static bool									IsArchive						(std::string aFileName);
		
	protected:
		fex_t*										Archive;
		std::string									FileName;
};

#endif
