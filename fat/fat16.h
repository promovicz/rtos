//Ryans Mods:
//	Added fat16_set_dir(...)-Allows user to set the next entry of the directory


/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef FAT16_H
#define FAT16_H

#include <stdint.h>

/**
 * \addtogroup fat16
 *
 * @{
 */
/**
 * \file
 * FAT16 header.
 *
 * \author Roland Riegel
 */

/**
 * \addtogroup fat16_file
 * @{
 */

/** The file is read-only. */
#define FAT16_ATTRIB_READONLY (1 << 0)
/** The file is hidden. */
#define FAT16_ATTRIB_HIDDEN (1 << 1)
/** The file is a system file. */
#define FAT16_ATTRIB_SYSTEM (1 << 2)
/** The file is empty and has the volume label as its name. */
#define FAT16_ATTRIB_VOLUME (1 << 3)
/** The file is a directory. */
#define FAT16_ATTRIB_DIR (1 << 4)
/** The file has to be archived. */
#define FAT16_ATTRIB_ARCHIVE (1 << 5)

/** The given offset is relative to the beginning of the file. */
#define FAT16_SEEK_SET 0
/** The given offset is relative to the current read/write position. */
#define FAT16_SEEK_CUR 1
/** The given offset is relative to the end of the file. */
#define FAT16_SEEK_END 2

/**
 * @}
 */

struct partition_struct;
struct fat16_fs_struct;
struct fat16_file_struct;
struct fat16_dir_struct;

/**
 * \ingroup fat16_file
 * Describes a directory entry.
 */
struct fat16_dir_entry_struct {
    /** The file's name, truncated to 31 characters. */
    char long_name[32];
    /** The file's attributes. Mask of the FAT16_ATTRIB_* constants. */
    uint8_t attributes;

    /** The time the file was last modified. */
    //uint16_t modified_time;
    /** The date the file was modified. */
    //uint16_t modified_date;

    /** The cluster in which the file's first byte resides. */
    uint16_t cluster;
    /** The file's size. */
    uint32_t file_size;
    /** The total disk offset of this directory entry. */
    uint32_t entry_offset;
};

/* Each entry within the directory table has a size of 32 bytes
 * and either contains a 8.3 DOS-style file name or a part of a
 * long file name, which may consist of several directory table
 * entries at once.
 *
 * multi-byte integer values are stored little-endian!
 *
 * 8.3 file name entry:
 * ====================
 * offset  length  description
 *      0       8  name (space padded)
 *      8       3  extension (space padded)
 *     11       1  attributes (FAT16_ATTRIB_*)
 
 *     0x0E     2  Creation Time
 *     0x10     2  Creation Date
 
 *
 * long file name (lfn) entry ordering for a single file name:
 * ===========================================================
 * LFN entry n
 *     ...
 * LFN entry 2
 * LFN entry 1
 * 8.3 entry (see above)
 *
 * lfn entry:
 * ==========
 * offset  length  description
 *      0       1  ordinal field
 *      1       2  unicode character 1
 *      3       3  unicode character 2
 *      5       3  unicode character 3
 *      7       3  unicode character 4
 *      9       3  unicode character 5
 *     11       1  attribute (always 0x0f)
 *     12       1  type (reserved, always 0)
 *     13       1  checksum
 *     14       2  unicode character 6
 *     16       2  unicode character 7
 *     18       2  unicode character 8
 *     20       2  unicode character 9
 *     22       2  unicode character 10
 *     24       2  unicode character 11
 *     26       2  cluster (unused, always 0)
 *     28       2  unicode character 12
 *     30       2  unicode character 13
 *
 * The ordinal field contains a descending number, from n to 1.
 * For the n'th lfn entry the ordinal field is or'ed with 0x40.
 * For deleted lfn entries, the ordinal field is set to 0xe5.
 */

struct fat16_header_struct {
    uint32_t size;

    uint32_t fat_offset;
    uint32_t fat_size;

    uint16_t sector_size;
    uint16_t cluster_size;

    uint32_t root_dir_offset;

    uint32_t cluster_zero_offset;
};

struct fat16_fs_struct {
    struct partition_struct* partition;
    struct fat16_header_struct header;
};

struct fat16_file_struct {
    unsigned int magic_cookie;
    struct fat16_fs_struct* fs;
    struct fat16_dir_entry_struct dir_entry;
    uint32_t pos;
    uint16_t pos_cluster;
};

struct fat16_dir_struct {
    struct fat16_fs_struct* fs;
    struct fat16_dir_entry_struct dir_entry;
    uint16_t entry_next;
};

struct fat16_read_callback_arg {
    uint16_t entry_cur;
    uint16_t entry_num;
    uint32_t entry_offset;
    uint8_t byte_count;
};

struct fat16_usage_count_callback_arg {
    uint16_t cluster_count;
    uint8_t buffer_size;
};


int fat16_open(struct fat16_fs_struct* fs, struct partition_struct* partition);

void 
fat16_close(struct fat16_fs_struct* fs);

int fat16_open_file(struct fat16_file_struct* fd, struct fat16_fs_struct* fs, const struct fat16_dir_entry_struct* dir_entry);

void 
fat16_close_file(struct fat16_file_struct* fd);

int16_t 
fat16_read_file(struct fat16_file_struct* fd, uint8_t* buffer, uint16_t buffer_len);

int16_t 
fat16_write_file(struct fat16_file_struct* fd, const uint8_t* buffer, uint16_t buffer_len);

uint8_t 
fat16_seek_file(struct fat16_file_struct* fd, int32_t* offset, uint8_t whence);

uint8_t 
fat16_resize_file(struct fat16_file_struct* fd, uint32_t size);

int fat16_open_dir(struct fat16_dir_struct* dd, struct fat16_fs_struct* fs, const struct fat16_dir_entry_struct* dir_entry);

void 
fat16_close_dir(struct fat16_dir_struct* dd);

uint8_t 
fat16_read_dir(struct fat16_dir_struct* dd, struct fat16_dir_entry_struct* dir_entry);

uint8_t 
fat16_reset_dir(struct fat16_dir_struct* dd);

uint8_t 
fat16_set_dir(struct fat16_dir_struct* dd, uint16_t offset);

uint8_t 
fat16_create_file(struct fat16_dir_struct* parent, const char* file, struct fat16_dir_entry_struct* dir_entry);

uint8_t 
fat16_delete_file(struct fat16_fs_struct* fs, struct fat16_dir_entry_struct* dir_entry);

uint8_t 
fat16_get_dir_entry_of_path(struct fat16_fs_struct* fs, const char* path, struct fat16_dir_entry_struct* dir_entry);

uint32_t 
fat16_get_fs_size(const struct fat16_fs_struct* fs);

uint32_t 
fat16_get_fs_free(const struct fat16_fs_struct* fs);

uint8_t 
find_file_in_dir(struct fat16_fs_struct* fs, struct fat16_dir_struct* dd, const char* name, struct fat16_dir_entry_struct* dir_entry);

int open_file_in_dir(struct fat16_file_struct* fd, struct fat16_fs_struct* fs, struct fat16_dir_struct* dd, const char* name);

int fat16_file_size(struct fat16_file_struct * file);

/**
 * @}
 */

#endif

