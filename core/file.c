
#include "file.h"

#include <stdio.h>

#include <core/defines.h>

/* pre-allocated descriptor structures */
struct file open_files[MAXFILES];

void file_table_init(void)
{
	/* clear all structures */
	memset(open_files, 0, sizeof(open_files));
}

struct file *file_alloc()
{
	int i;
	for (i = 0; i < MAXFILES; i++) {
		if (!open_files[i].f_used) {
			open_files[i].f_used = -1;
			return &open_files[i];
		}
	}

	errno = ENFILE;
	return NULL;
}

int file_ref(struct file *fd)
{
	if (fd->f_used == -1) {
		fd->f_used = 1;
	} else {
		fd->f_used++;
	}
	return fd->f_used;
}

int file_unref(struct file *fd)
{
	if (fd->f_used <= 0) {
		fd->f_used = 0;
	} else {
		fd->f_used--;
	}
	return fd->f_used;
}

int file_sync(struct file *fd)
{
	/* pass through to the implementation, if present. else fail. */
	if (fd->f_ops->fop_sync) {
		return fd->f_ops->fop_sync(fd);
	} else {
		errno = EOPNOTSUPP;
		return -1;
	}
}

off_t file_seek(struct file * fd, off_t offset, int whence)
{
	/* pass through to the implementation, if present. else fail. */
	if (fd->f_ops->fop_seek) {
		return fd->f_ops->fop_seek(fd, offset, whence);
	} else {
		errno = EOPNOTSUPP;
		return -1;
	}
}

size_t file_read(struct file * fd, void *buf, size_t count)
{
	/* pass through to the implementation, if present. else fail. */
	if (fd->f_ops->fop_read) {
		return fd->f_ops->fop_read(fd, buf, count);
	} else {
		errno = EOPNOTSUPP;
		return -1;
	}
}

size_t file_write(struct file * fd, const void *buf, size_t count)
{
	/* pass through to the implementation, if present. else fail. */
	if (fd->f_ops->fop_write) {
		return fd->f_ops->fop_write(fd, buf, count);
	} else {
		errno = EOPNOTSUPP;
		return -1;
	}
}

int file_close(struct file *fd)
{
	/* unref and really close if at last ref */
	if (!file_unref(fd)) {
		return 0;
	}

	/* call object implementation once */
	if (fd->f_ops && fd->f_ops->fop_close) {
		return fd->f_ops->fop_close(fd);
	}

	/* clear structure */
	memset(fd, 0, sizeof(struct file));

	/* done */
	return 0;
}
