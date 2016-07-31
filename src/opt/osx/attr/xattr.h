#ifndef _ATTR_XTTR_OSX_H
#define _ATTR_XTTR_OSX_H
#ifdef __APPLE__

#include <string>
#include <sys/xattr.h>
#include <errno.h>

// XXX: Workaround to enable copying with Finder.
#define XATTR_IGNORE(name) { \
	 if (strcmp(name, "com.apple.FinderInfo") == 0 ) { \
		 return -ENOTSUP; \
	 } \
}

inline
int __setxattr(const char *path, const char *name, const void *value, size_t size, u_int32_t position, int options) {
	XATTR_IGNORE(name)
	return setxattr(path, name, value, size, position, options);
}

inline
int __fsetxattr(int fd, const char *name, const void *value, size_t size, u_int32_t position, int options) {
	XATTR_IGNORE(name)
	return fsetxattr(fd, name, value, size, position, options);
}

inline
int setxattr(const char *path, const char *name, const void *value, size_t size, int flags) {
	return __setxattr(path, name, value, size, 0, flags & ~XATTR_NOFOLLOW);
}
#define lsetxattr(path, name, value, size, flags)		__setxattr(path, name, value, size, 0, flags |  XATTR_NOFOLLOW)
#define fsetxattr(fd, name, value, size, flags)			__fsetxattr(fd, name, value, size, 0, flags & ~XATTR_NOFOLLOW)


inline
ssize_t getxattr(const char *path, const char *name, void *value, size_t size) {
	return getxattr(path, name, value, size, 0, 0);
}
#define lgetxattr(path, name, value, size)				getxattr(path, name, value, size, 0, XATTR_NOFOLLOW)
#define fgetxattr(fd,  name, value, size)				fgetxattr(fd, name, value, size, 0, 0)


inline
ssize_t listxattr(const char *path, char *list, size_t size) {
	return listxattr(path, list, size, 0);
}
#define llistxattr(path, list, size)					listxattr(path, list, size, XATTR_NOFOLLOW)
#define flistxattr(fd, list, size)						flistxattr(fd, list, size, 0)

inline
int removexattr(const char *path, const char *name) {
	return removexattr(path, name, 0);
}
#define lremovexattr(path, name)						removexattr(path, name, XATTR_NOFOLLOW)
#define fremovexattr(fd, name)							fremovexattr(fd, name, 0)

#endif /* __APPLE__ */
#endif /* _ATTR_XTTR_OSX_H */

