// include at the end of sol multi-headers like sol.hpp -- not recursively composable!
#ifdef SOL_GLOBAL_HAS_BEGUN
#undef SOL_GLOBAL_HAS_BEGUN
#include "file_end.hpp"
#else
#error "sol/global_end.hpp" was included without including "sol/global_begin.hpp" beforehand; something went wrong!
#endif // SOL_GLOBAL_HAS_BEGUN
