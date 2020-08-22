/* local.h - fine the -lndir include file */

#ifndef	BSD42
#include <sys/types.h>
#else	BSD42
#include <sys/param.h>
#endif	BSD42

#ifndef	BSD42
#ifndef	SYS5DIR
#ifndef NDIR
#include <dir.h>
#else	NDIR
#include <ndir.h>
#endif  NDIR
#else	SYS5DIR
#include <dirent.h>
#endif	SYS5DIR
#else	BSD42
#include <sys/dir.h>
#endif	BSD42

#include <sys/stat.h>
