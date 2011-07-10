
#include "filesystem.h"

#include <core/file.h>

#define SFS_MAXFNAME 8
#define SFS_MAXLABEL 8

struct sfs_file {
	size_t size;
	uint8_t content[];
};

struct sfs_dirent {
	char name[SFS_MAXFNAME];
	char type;
	union {
		struct sfs_file *file;
		struct sfs_dir *dir;
	} object;
};

struct sfs_dir {
	size_t size;
	struct sfs_dirent entries[];
};

struct sfs {
	char label[SFS_MAXLABEL];
	struct sfs_dir *root_directory;
};



const struct sfs_file example_file = {
		.size = 6,
		.content = "example",
};

const struct sfs_dir root_dir = {
		.size = 1,
		.entries = {
				{.name = "example", .type = 'f', .object.file = &example_file},
		},
};

struct sfs root_sfs = {
		.label = "root",
		.root_directory = &root_dir,
};

off_t sfs_file_seek (struct file * fd, off_t offset, int whence)
{
	struct sfs_file *f = (struct sfs_file *)fd->f_cookie_ptr;

	switch(whence) {
	case SEEK_SET:
		if(offset < 0) {
			return -EINVAL;
		}
		fd->f_offset = offset;
		break;
	case SEEK_CUR:
		fd->f_offset = fd->f_offset + offset;
		break;
	case SEEK_END:
		fd->f_offset = f->size + offset;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

ssize_t sfs_file_read (struct file * fd, void *buf, size_t size)
{
	struct sfs_file *f = (struct sfs_file *)fd->f_cookie_ptr;
	size_t bavail, bread;

	if((size_t)fd->f_offset > f->size) {
		return -EFAULT;
	}

	bavail = f->size - fd->f_offset;

	if(!bavail) {
		return 0;
	}

	bread = (bavail > size) ? size : bavail;

	memcpy(buf, f->content, bread);

	return bread;
}

const struct file_operations sfs_file_operations = {
		.fop_read = &sfs_file_read,
		.fop_seek = &generic_seek_file,
};

int sfs_dir_readdir (struct file *fd, void *cookie, fop_filldir_t fill)
{
	struct sfs_dir *d = (struct sfs_dir *)fd->f_cookie_ptr;
	size_t x;

	for(x = 0; x < d->size; x++) {
		struct sfs_dirent *e = &d->entries[x];
		fill(fd, e->name, e->);
	}

	return d->size;
}

const struct file_operations sfs_dir_operations = {
		.fop_read = &generic_read_readdir,
		.fop_seek = &generic_seek_file,
		.fop_readdir = &sfs_dir_readdir,
};
