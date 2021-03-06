#ifndef CORE_FILE_H
#define CORE_FILE_H

#include <core/common.h>

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

typedef void(*fop_filldir_t) (void *cookie,
	char *fname, size_t fsize, ino_t finode, char ftype);

/* function pointer types for operations */
typedef int (*fop_open_t) (struct file * fd);
typedef int (*fop_sync_t) (struct file * fd);
typedef off_t(*fop_seek_t) (struct file * fd, off_t offset, int whence);
typedef ssize_t(*fop_read_t) (struct file * fd, void *buf, size_t size);
typedef int(*fop_readdir_t) (struct file *fd, void *cookie, fop_filldir_t fill);
typedef ssize_t(*fop_write_t) (struct file * fd, const void *buf, size_t size);
typedef int (*fop_close_t) (struct file * fd);

/* structure describing operations for file */
struct file_operations {
	fop_open_t    fop_open;
	fop_sync_t    fop_sync;
	fop_seek_t    fop_seek;
	fop_read_t    fop_read;
	fop_readdir_t fop_readdir;
	fop_write_t   fop_write;
	fop_close_t   fop_close;
};

/* structure describing the state behind a file descriptor */
struct file {
	/* refcount, -1 used on allocation */
	int8_t f_used;
	/* file flags (O_RDONLY and so forth) */
	int f_flags;
	/* descriptive name */
	char *f_name;
	/* current seek offset */
	off_t f_offset;
	/* object-specific operation structure */
	struct file_operations *f_ops;
	/* object-specific file structure */
	union {
		void *f_cookie_ptr;
		int f_cookie_int;
		struct device *f_device;
	};
};

/* "table" of all file descriptors */
extern struct file *open_fds[MAXFDS];

/* pre-allocated descriptor structures */
extern struct file open_files[MAXFILES];

/* initializer for the file subsystem */
void file_table_init(void);

/* debug: report file table stats */
void file_table_report(void);

/* file object operations */
struct file *file_alloc();

int file_ref(struct file *fd);
int file_unref(struct file *fd);
int file_sync(struct file *fd);
off_t file_seek(struct file * fd, off_t offset, int whence);
size_t file_read(struct file * fd, void *buf, size_t count);
size_t file_write(struct file * fd, const void *buf, size_t count);
int file_close(struct file *fd);

off_t generic_seek_file (struct file * fd, off_t offset, int whence);
ssize_t generic_read_readdir (struct file *fd, void *buf, size_t size);

/* file descriptor operations */
int fd_alloc(struct file *file);
struct file *file_for_fd(int fd);

#endif /* !CORE_FILE_H */
