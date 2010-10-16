
#include <core/defines.h>

#include <posix/file.h>

#include <sys/epoll.h>

#define EPOLL_MAX (2)
#define EPOLL_MAXFDS (8)

struct epoll_entry {
	int fd;
	struct epoll_event event;
};

struct epoll_file {
	struct file *file;
	struct epoll_entry entries[EPOLL_MAXFDS];
};

struct epoll_file files[EPOLL_MAX];

struct file_operations epoll_operations;

int _epoll_create(int ignored)
{
	int i, fd;
	struct epoll_file *ef = NULL;
	struct file *f = NULL;

	/* find epoll_file struct */
	for(i = 0; i < EPOLL_MAXFDS; i++) {
		if(!files[i].file) {
			ef = &files[i];
			break;
		}
	}
	if(!ef) {
		errno = ENOMEM;
		return -1;
	}

	/* allocate file struct */
	f = file_alloc();
	if(!f) {
		errno = ENOMEM;
		return -1;
	}

	/* allocate fd */
	fd = fd_alloc(f);
	if(fd == -1) {
		file_close(f);
		errno = EMFILE;
		return -1;
	}

	/* got everything, fill out structures */
	f->f_ops = &epoll_operations;
	f->f_cookie_ptr = (void *)ef;
	ef->file = f;
	for(i = 0; i < EPOLL_MAXFDS; i++) {
		ef->entries[i].fd = -1;
	}

	/* return fd */
	return fd;
}

int epoll_create(int size)
	weak_alias("_epoll_create");

int epoll_create1(int size)
	weak_alias("_epoll_create");


int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
	int i;
	struct file *f;
	struct file *fdf;
	struct epoll_file *ef;
	struct epoll_entry *ee = NULL;

	/* check fd */
	fdf = file_for_fd(fd);
	if(!fdf) {
		/* pass errno */
		return -1;
	}

	/* check epfd */
	f = file_for_fd(epfd);
	if(!f) {
		/* pass errno */
		return -1;
	}

	if(f->f_ops != &epoll_operations) {
		errno = EINVAL;
		return -1;
	}

	/* get epoll_file */
	if(!f->f_cookie_ptr) {
		errno = EFAULT; // XXX ???
		return -1;
	}
	ef = (struct epoll_file *)f->f_cookie_ptr;

	/* perform operation */
	if(op == EPOLL_CTL_ADD) {
		/* check event */
		if(!event) {
			errno = EFAULT;
			return -1;
		}

		/* find available entry or duplicate */
		for(i = 0; i < EPOLL_MAXFDS; i++) {
			if(ef->entries[i].fd == fd) {
				errno = EEXIST;
				return -1;
			} else if (ef->entries[i].fd == -1) {
				ee = &ef->entries[i];
			}
		}
		if(!ee) {
			errno = ENOMEM;
			return -1;
		}
		/* got everything, fill out and succeed */
		memcpy(&ee->event, event, sizeof(struct epoll_event));
		ee->fd = fd;
	} else if (op == EPOLL_CTL_MOD) {
		/* check event */
		if(!event) {
			errno = EFAULT;
			return -1;
		}

		/* find existing entry */
		for(i = 0; i < EPOLL_MAXFDS; i++) {
			if(ef->entries[i].fd == fd) {
				ee = &ef->entries[i];
			}
		}
		if(!ee) {
			errno = ENOENT;
			return -1;
		}
		/* update the entry */
		memcpy(&ee->event, event, sizeof(struct epoll_event));
	} else if (op == EPOLL_CTL_DEL) {
		/* find existing entry */
		for(i = 0; i < EPOLL_MAXFDS; i++) {
			if(ef->entries[i].fd == fd) {
				ee = &ef->entries[i];
			}
		}
		if(!ee) {
			errno = ENOENT;
			return -1;
		}
		/* clear the entry */
		memset(&ee->event, 0, sizeof(struct epoll_event));
		ee->fd = -1;
	}

	return 0;
}

int epoll_wait(int epfd,
			   struct epoll_event *events,
			   int maxevents,
			   int timeout)
{
	errno = EFAULT;
	return -1;
}

int epoll_pwait(int epfd,
			   struct epoll_event *events,
			   int maxevents,
			   int timeout,
			   const sigset_t *sigmask)
{
	return epoll_wait(epfd, events, maxevents, timeout);
}
