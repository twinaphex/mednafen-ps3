#if defined(__i386__)
# include "linkage_x86.s"
#elif defined(__x86_64__)
# include "linkage_x64.s"
#else
# error "Unsupported platform for recompiler"
#endif

