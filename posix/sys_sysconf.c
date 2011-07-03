
#include "common.h"

#include <core/memory.h>

#include <errno.h>
#include <unistd.h>

long sysconf(int name)
{
	switch(name) {
#if 0
	case _SC_ARG_MAX:
		return _POSIX_ARG_MAX;
	case _SC_CHILD_MAX:
		return _POSIX_CHILD_MAX;
	case _SC_HOST_NAME_MAX:
		return _POSIX_HOST_NAME_MAX;
	case _SC_LOGIN_NAME_MAX:
		return _POSIX_LOGIN_NAME_MAX;
	case _SC_CLK_TCK:
		return _POSIX_CLK_TCK;
	case _SC_OPEN_MAX:
		return _POSIX_OPEN_MAX;
	case _SC_RE_DUP_MAX:
		return _POSIX_RE_DUP_MAX;
	case _SC_STREAM_MAX:
		return _POSIX_STREAM_MAX;
	case _SC_SYMLOOP_MAX:
		return _POSIX_SYMLOOP_MAX;
	case _SC_TTY_NAME_MAX:
		return _POSIX_TTY_NAME_MAX;
	case _SC_TZNAME_MAX:
		return _POSIX_TZNAME_MAX;
	case _SC_VERSION:
		return 199009L; // XXX
#endif

	case _SC_PAGESIZE:
		return PAGESIZE;


	default:
		errno = -EINVAL;
		return -1;
	}

	return 0;
}
