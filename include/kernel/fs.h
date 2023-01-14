/**
 * fs.h
 * This file contains APIs of filesystem, it's used inside the kernel.
 * There is a seperate header file for user program's use.
 * This file is added by xw. 18/6/17
 */

#ifndef _FS_H_
#define _FS_H_

#include <common/type.h>
#include <define/define.h>

extern struct file_desc f_desc_table[NR_FILE_DESC];  // in fs.c

void init_fs();

// added by mingxuan 2019-5-17
int real_open(const char *pathname, int flags);
int real_close(int fd);
int real_read(int fd, void *buf, int count);
int real_write(int fd, const void *buf, int count);
int real_unlink(const char *pathname);
int real_lseek(int fd, int offset, int whence);

// added by mingxuan 2020-10-30
void                read_super_block(int dev);
struct super_block *get_super_block(int dev);
int                 get_fs_dev(int drive, int fs_type);

#endif /* _FS_H_ */
