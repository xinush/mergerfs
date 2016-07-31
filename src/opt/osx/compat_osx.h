#ifndef _COMPAT_OSX_H
#define _COMPAT_OSX_H
#ifdef __APPLE__

#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


#define		st_atim				st_atimespec
#define		st_mtim				st_mtimespec
#define		st_ctim				st_ctimespec
#define		st_birthtim			st_birthtimespec

#ifndef 	FS_IOC_GETFLAGS
# define	FS_IOC_GETFLAGS		_IOR('f', 1, long)
# define	FS_IOC_SETFLAGS		_IOW('f', 2, long)
#endif

#ifndef 	O_LARGEFILE
# define 	O_LARGEFILE 0
#endif

#ifndef 	O_NOATIME
# define 	O_NOATIME 0
#endif


inline
int getgrouplist(const char *name, int basegid, gid_t *groups, int *ngroups)
{
	return getgrouplist(name, basegid, (int*) groups, ngroups);
}


#ifdef WITH_GNULIB
# define 	eaccess(pathname, mode)    	euidaccess(pathname, mode)
#else
# define 	eaccess(pathname, mode)    	faccessat(AT_FDCWD, pathname, mode, AT_EACCESS);
#endif


#ifndef WITH_GNULIB
inline
int futimens(int fd, const struct timespec ts[2])
{
    struct timeval tv[2];
    TIMESPEC_TO_TIMEVAL(&tv[0], &ts[0]);
    TIMESPEC_TO_TIMEVAL(&tv[1], &ts[1]);

    return ::futimes(fd,tv);
}
#endif


#ifndef WITH_GNULIB
inline
int utimensat(int dirfd, const char *pathname, const struct timespec ts[2], int flags)
{
	if (dirfd > 0) {
		return (errno=ENOTSUP, -1);
	}

    struct timeval tv[2];
    TIMESPEC_TO_TIMEVAL(&tv[0], &ts[0]);
    TIMESPEC_TO_TIMEVAL(&tv[1], &ts[1]);

    int rv = -1;
    if ((flags & AT_SYMLINK_NOFOLLOW) == AT_SYMLINK_NOFOLLOW) {
    	rv = ::lutimes(pathname,tv);
    }
    else {
    	rv = ::utimes(pathname,tv);
    }

    return rv;
}
#endif


#ifndef WITH_GNULIB
inline
int fdatasync(int fd)
{
	return (::fcntl(fd, F_FULLFSYNC) == -1) ? ::fsync(fd) : 0;
}
#endif 


#define 	POSIX_FADV_WILLNEED			F_RDADVISE
#define 	POSIX_FADV_SEQUENTIAL		F_RDAHEAD

inline
int posix_fadvise(int fd, off_t offset, off_t len, int advice)
{
	int rv = -1;
	if (advice == F_RDADVISE) {
		struct radvisory rd = {offset, len};
		rv = ::fcntl(fd, F_RDADVISE, &rd);
	}
	else if (advice == F_RDAHEAD) {
		rv = ::fcntl(fd, F_RDAHEAD, 1);
	}
	else {
		return ENOTSUP;
	}
	return (rv != -1) ? 0 : errno;
}

inline
int fallocate(int fd, int mode, off_t offset, off_t len)
{
	if (mode) {
		return (errno=ENOTSUP, -1);
	}

	fstore_t fst;
	fst.fst_flags = F_ALLOCATECONTIG;
	fst.fst_posmode = F_PEOFPOSMODE;
	fst.fst_offset = offset;
	fst.fst_length = len;
	fst.fst_bytesalloc = 0;

	int rv = -1;
	if (::fcntl(fd,F_PREALLOCATE,&fst) == -1) {
		fst.fst_posmode = F_ALLOCATEALL;
		rv = (::fcntl(fd, F_PREALLOCATE, &fst) == -1) ? -1 : 0;
	}

	if (rv != -1) {
		rv = ::ftruncate(fd, len);
	}
	return rv;
}


#include "attr/xattr.h"
#include "../../getxattr.hpp"
#include "../../setxattr.hpp"

namespace mergerfs
{
  namespace fuse
  {
  	  inline
	  int getxattr(const char* fusepath, const char* attrname, char* buf, size_t count, u_int32_t position)
  	  {
  		  return getxattr(fusepath, attrname, buf, count);
  	  }

      inline
	  int setxattr(const char* fusepath, const char* attrname, const char* attrval, size_t attrvalsize, int flags, uint32_t position)
      {
      	return setxattr(fusepath, attrname, attrval, attrvalsize, flags);
      }
  }
}

#endif /* __APPLE__ */
#endif /* _COMPAT_OSX_H */
