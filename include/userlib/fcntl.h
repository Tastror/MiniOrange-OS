#ifndef __USER_FCNTL_H__
#define __USER_FCNTL_H__

#include <common/type.h>
#include <define/define.h>

int open(const char *pathname, int flags);
int close(int fd);
int read(int fd, void *buf, int count);
int write(int fd, const void *buf, int count);
int lseek(int fd, int offset, int whence);
int unlink(const char *pathname);

#endif