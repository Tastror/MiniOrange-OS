#ifndef __USER_FCNTL_H__
#define __USER_FCNTL_H__

#include <common/type.h>
#include <define/define.h>

int open(const char *pathname, int flags);      // added by xw, 18/6/19
int close(int fd);                              // added by xw, 18/6/19
int read(int fd, void *buf, int count);         // added by xw, 18/6/19
int write(int fd, const void *buf, int count);  // added by xw, 18/6/19
int lseek(int fd, int offset, int whence);      // added by xw, 18/6/19
int unlink(const char *pathname);               // added by xw, 18/6/19

#endif