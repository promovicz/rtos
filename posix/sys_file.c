
#include "file.h"

/* dummy open implementation */
int __libc_open(const char *pathname, int flags, ...)
{
	errno = EOPNOTSUPP;
	return -1;
}

int open(const char *pathname, int flags, ...)
	weak_alias("__libc_open");


/* close implementation */
int __libc_close(int fd)
{
	struct file *file = file_for_fd(fd);
	if (!file) {
		errno = EBADF;
		return -1;
	}

	int res = file_close(file);
	if (res < 0) {
		return res;
	}

	open_fds[fd] = NULL;

	return 0;
}

int close(int fd)
	weak_alias("__libc_close");


/* read implementation */
ssize_t __libc_read(int fd, void *buf, size_t count)
{
	struct file *f = file_for_fd(fd);
	if (!f) {
		errno = EBADF;
		return -1;
	}

	return file_read(f, buf, count);
}

ssize_t read(int fd, void *buf, size_t count)
	weak_alias("__libc_read");


/* write implementation */
ssize_t __libc_write(int fd, const void *buf, size_t count)
{
	struct file *f = file_for_fd(fd);
	if (!f) {
		errno = EBADF;
		return -1;
	}

	return file_write(f, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count)
	weak_alias("__libc_write");


/* seek implementation */
off_t __libc_lseek(int fd, off_t offset, int whence)
{
	struct file *f = file_for_fd(fd);
	if (!f) {
		errno = EBADF;
		return -1;
	}

	return file_seek(f, offset, whence);
}

off_t lseek(int fd, off_t offset, int whence)
	weak_alias("__libc_lseek");


/* dummy fstat implementation */
int __libc_fstat(int fd, struct stat *buf)
{
	struct file *f = file_for_fd(fd);
	if (!f) {
		errno = EBADF;
		return -1;
	}

	errno = EOPNOTSUPP;
	return -1;
}

int fstat(int fd, struct stat *buf)
	weak_alias("__libc_fstat");


/* dummy ioctl implementation */
int __libc_ioctl(int fd, long int request, ...)
{
	struct file *f = file_for_fd(fd);
	if (!f) {
		errno = EBADF;
		return -1;
	}

	errno = EOPNOTSUPP;
	return -1;
}

int ioctl(int fd, long int request, ...)
	weak_alias("__libc_ioctl");
