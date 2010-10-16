#ifndef POSIX_CONTROL_H
#define POSIX_CONTROL_H

#include <core/types.h>

#include <fcntl.h>
#include <errno.h>
#include <string.h>

/* max number of file descriptors (would-be per-process) */
#define MAXFDS 8
/* max number of open files */
#define MAXFILES 8

/* forward declarations */
struct file;
struct file_operations;

/* function pointer types for operations */
typedef int (*fop_sync_t) (struct file * fd);
typedef off_t(*fop_seek_t) (struct file * fd, off_t offset, int whence);
typedef ssize_t(*fop_read_t) (struct file * fd, void *buf, size_t size);
typedef ssize_t(*fop_write_t) (struct file * fd, const void *buf, size_t size);
typedef int (*fop_close_t) (struct file * fd);

/* structure describing operations for file */
struct file_operations {
	fop_sync_t fop_sync;
	fop_seek_t fop_seek;
	fop_read_t fop_read;
	fop_write_t fop_write;
	fop_close_t fop_close;
};

/* structure describing the state behind a file descriptor */
struct file {
	/* refcount, -1 used on allocation */
	int8_t f_used;
	/* file flags (O_RDONLY and so forth) */
	int f_flags;
	/* object-specific operation structure */
	struct file_operations *f_ops;
	/* object-specific file structure */
	union {
		void *f_cookie_ptr;
		int f_cookie_int;
	}
};

void file_table_init(void);

struct file *file_alloc();
int file_ref(struct file *fd);
int file_unref(struct file *fd);
int file_close(struct file *fd);

int fd_alloc(struct file *file);
struct file *file_for_fd(int fd);

#endif /* !POSIX_CONTROL_H */
