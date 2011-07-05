#include "fat16.h"
#include "rootdir.h"
#include "partition.h"
#include "sd_raw.h"
#include <stdio.h>

struct fat16_dir_entry_struct dir_entry;
struct fat16_fs_struct fs;
struct partition_struct partition;
struct fat16_dir_struct dd;

//return 0 on success, negative on failure
//-1 - can't open partition
//-2 - can't open filesystem (not fat16?)
//-3 - can't open root directory
int openroot(void) {
    /* open first partition */
    int result=partition_open(&partition,(device_read_t) sd_raw_read,
                               (device_read_interval_t) sd_raw_read_interval,
                               (device_write_t) sd_raw_write,
                               0);

    if(result<0) {
        /* If the partition did not open, assume the storage device
             *      * is a "superfloppy", i.e. has no MBR.
             *           */
        result = partition_open(&partition, (device_read_t) sd_raw_read,
                                   (device_read_interval_t) sd_raw_read_interval,
                                   (device_write_t) sd_raw_write,
                                   -1);
        if(result<0) return -1;
    }

    /* open file system */
    result = fat16_open(&fs,&partition);
    if(result<0) return -2;

    /* open root directory */
    result=fat16_get_dir_entry_of_path(&fs, "/", &dir_entry);

    result = fat16_open_dir(&dd,&fs, &dir_entry);
    if(result<0) {
        return -3;
    }
    return 0;
}

/* returns 1 if file exists, 0 else */
int root_file_exists(char* name) {
    return(find_file_in_dir(&fs,&dd,name,&dir_entry));
}

/* returns NULL if error, pointer if file opened */
int root_open_new(struct fat16_file_struct* fd, char* name) {
    if(fat16_create_file(&dd,name,&dir_entry)) {
        return(open_file_in_dir(fd,&fs,&dd,name));
    } else {
        return -1;
    }
}

int root_open(struct fat16_file_struct* fd, char* name) {
    return(open_file_in_dir(fd,&fs,&dd,name));
}

/* sequential calls return sequential characters
 * of the sequence of file names in the rootdir
 * in place of '\0' it returns ',' only
 * returning a zero when the end of all files
 * has been reached.
 *
 * Assert (1) reset whenever you want to re-start
 */
char rootDirectory_files_stream(int reset) {

    static int idx = 0;

    /* If reset, we need to reset the dir */
    if(reset) {
        fat16_reset_dir(&dd);
        return 0;
    }

    /* Whenever IDX is zero, we're gonna start a new file,
       * so read a new one.
       * if there's no new file,
       * return 0, because it's over
       */
    if(idx == 0)  {
        if(fat16_read_dir(&dd,&dir_entry)==0) {
            return '\0';
        }
    }

    /* If we've reached the end of a string,
       * return comma instead of \0,
       * so the list is comma delimited,
       * and terminated with a zero
       */
    if(dir_entry.long_name[idx]=='\0') {
        idx = 0;
        return ',';
    }


    return dir_entry.long_name[idx++];

}
//Description: Fills buf with len number of chars.  Returns the number of files
//				that were cycled through during the read
//Pre: buf is an array of characters at least as big as len
//		len is the size of the array to read
//Post: buf contains the characters of the filenames in Root, starting at the first file
//		and ending after len characters
int rootDirectory_files(char* buf, int len) {
    int i;
    int num=0;
    /* Loop will walk through every file in directory dd */
    fat16_reset_dir(&dd);
    while(fat16_read_dir(&dd,&dir_entry)) {
        i = 0;
        /* Spin through the filename */
        while(dir_entry.long_name[i]!='\0') {
            /* And copy each character into buf */
            *buf++=dir_entry.long_name[i++];
            len--;
            if(len==1) {
                /* Buf if we ever get to the end of buf, quit */
                *buf='\0';
                return 1;
            }
        }
        *buf++=',';
        num++;
        len--;
        if(len==1) {
            /* Buf if we ever get to the end of buf, quit */
            *buf='\0';
            return 1;
        }
    }
    *buf='\0';
    return num;
}

void root_format(void) {
    fat16_reset_dir(&dd);
    while(fat16_read_dir(&dd,&dir_entry)) {
        fat16_delete_file(&fs,&dir_entry);
        fat16_reset_dir(&dd);
    }
}

int root_delete(char* filename) {
    if(find_file_in_dir(&fs,&dd,filename,&dir_entry)) {
        fat16_delete_file(&fs,&dir_entry);
        return 0;
    }
    return 1;
}
