#ifndef ROOTDIR_H
#define ROOTDIR_H

#include "fat16.h"
#include <stdint.h>

uint8_t print_disk_info(const struct fat16_fs_struct* fs);

/* returns NULL if error, pointer if file opened */
int root_open_new(struct fat16_file_struct* fd, char* name);

/* returns 1 if file exists, 0 else */
int root_file_exists(char* name);

int openroot(void);

int root_open(struct fat16_file_struct* fd, char* name);

void root_disk_info(void);
int rootDirectory_files(char* buf, int len);
void root_format(void);
char rootDirectory_files_stream(int reset);
int root_delete(char* filename);

#endif
