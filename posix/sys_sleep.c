
#include <core/defines.h>
#include <core/timer.h>
#include <core/tick.h>

#include <errno.h>
#include <time.h>

int __libc_nanosleep(const struct timespec *req, struct timespec *rem)
{
	nanosecs_t t;

	if(!req) {
		errno = EFAULT;
		return -1;
	}

	if(!(req->tv_sec || req->tv_nsec)) {
		return 0;
	}

	if(req->tv_sec < 0) {
		errno = EINVAL;
		return -1;
	}

	if(req->tv_nsec < 0 || req->tv_nsec > 999999999) {
		errno = EINVAL;
		return -1;
	}

	t = req->tv_sec * NANOSECS_SEC + req->tv_nsec;

	if(t < 1) {
		t = 1;
	}

	timer_sleep(t);

	return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem)
	weak_alias("__libc_nanosleep");
