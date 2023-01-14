/**
 * fat32.h
 *
 * mingxuan
 * 2019-5-17
 */

#ifndef _FAT32_H_
#define _FAT32_H_

#include <common/type.h>
#include <define/define.h>

STATE CreateVDisk(DWORD size);
STATE FormatVDisk(PCHAR path, PCHAR volumelabel);
STATE LoadVDisk(PCHAR path);
STATE CreateDir(const char *dirname);
STATE CreateFile(const char *filename);
STATE OpenFile(const char *filename, int mode);
STATE CloseFile(int fd);
STATE OpenDir(const char *dirname);

STATE ReadFile(int fd, void *buf, int length);
STATE WriteFile(int fd, const void *buf, int length);
STATE CopyFileIn(PCHAR sfilename, PCHAR dfilename);
STATE CopyFileOut(PCHAR sfilename, PCHAR dfilename);
STATE DeleteFile(const char *filename);
STATE DeleteDir(const char *dirname);
STATE ListAll(PCHAR dirname, DArray *array);
STATE IsFile(PCHAR path, PUINT tag);
STATE GetFileLength(PCHAR filename, PDWORD length);
STATE Rename(PCHAR path, PCHAR newname);
STATE CopyFile(PCHAR sfilename, PCHAR dpath);
STATE CopyDir(PCHAR sdirname, PCHAR dpath);
STATE Move(PCHAR sfilename, PCHAR dpath);
STATE GetProperty(PCHAR filename, Properties *properties);
STATE GetParenetDir(PCHAR name, PCHAR parentDir);
STATE GetVDiskFreeSpace(PDWORD left);
STATE GetVDiskSize(PDWORD totalsize);
STATE GetCurrentPath(PCHAR path);

DArray     *InitDArray(UINT initialCapacity, UINT capacityIncrement);
void        AddElement(DArray *array, DArrayElem element);
DArrayElem *NextElement(DArray *array);
void        DestroyDArray(DArray *array);

void  TimeToBytes(WORD result[]);                        // t 是一个 WORD 类型的数组，长度为 2，第 0 个元素为日期，第 1 个元素为时间
void  BytesToTime(WORD date, WORD time, WORD result[]);  // t 是一个表示时间的 WORD 数组，长度为 6，分别表示年、月、日、时、分、秒
void  TimeToString(WORD result[], PCHAR timestring);
STATE IsFullPath(PCHAR path);                                       // 判断 path 是否是一个绝对路径
void  ToFullPath(PCHAR path, PCHAR fullpath);                       // 将 path 转换成绝对路径，用 fullpath 返回
void  GetParentFromPath(PCHAR fullpath, PCHAR parent);              // 从一个绝对路径中得到它的父路径（父目录）
void  GetNameFromPath(PCHAR path, PCHAR name);                      // 从一个路径中得到文件或目录的名称
void  MakeFullPath(PCHAR parent, PCHAR name, PCHAR fullpath);       // 将父路径和文件名组合成一个绝对路径
void  FormatFileNameAndExt(PCHAR filename, PCHAR name, PCHAR ext);  // 将一个文件名转换成带空格的文件名和后缀名的形式，为了写入目录项。
void  FormatDirNameAndExt(PCHAR dirname, PCHAR name, PCHAR ext);    // 将一个目录名转换成带空格的目录名和后缀的形式，为了写入目录项。
void  ChangeCurrentPath(PCHAR addpath);

void  GetNameFromRecord(Record record, PCHAR fullname);                                                       // 从目录项中得到文件或目录的全名
STATE PathToCluster(PCHAR path, PDWORD cluster);                                                              // 将抽象的路径名转换成簇号
STATE FindSpaceInDir(DWORD parentCluster, PCHAR name, PDWORD sectorIndex, PDWORD off_in_sector);              // 在指定的目录中寻找空的目录项
STATE FindClusterForDir(PDWORD pcluster);                                                                     // 为目录分配簇
STATE ReadRecord(DWORD parentCluster, PCHAR name, PRecord record, PDWORD sectorIndex, PDWORD off_in_sector);  // 获得指定的目录项的位置（偏移量）
STATE WriteRecord(Record record, DWORD sectorIndex, DWORD off_in_sector);
STATE FindClusterForFile(DWORD totalClusters, PDWORD clusters);  // 为一个文件分配totalClusters个簇
STATE WriteFAT(DWORD totalClusters, PDWORD clusters);            // 写FAT1和FAT2
STATE GetNextCluster(DWORD curClusterIndex, PDWORD nextClusterIndex);
STATE AddCluster(DWORD startCluster, DWORD num);                                                 // cluster 表示该文件或目录所在目录的簇，num 表示增加几个簇
void  CreateRecord(PCHAR filename, BYTE type, DWORD startCluster, DWORD size, PRecord precord);  // 创建一个目录项
void  GetFileOffset(PFile pfile, PDWORD sectorIndex, PDWORD off_in_sector, PUINT isLastSector);  // 将文件当前位置转换成相对虚拟磁盘文件起始地址的偏移量，便于读写。
STATE AllotClustersForEmptyFile(PFile pfile, DWORD size);                                        // 为一个打开的空文件分配簇。
STATE NeedMoreCluster(PFile pfile, DWORD size, PDWORD number);                                   // 判断是否需要更多的簇，如果需要就返回需要的簇的个数
STATE ClearRecord(DWORD parentCluster, PCHAR name, PDWORD startCluster);                         // 删除记录项
void  ClearFATs(DWORD startcluster);                                                             // 删除簇号
STATE FindNextRecord(PDWORD cluster, PDWORD off, PCHAR name, PUINT tag);
void  DeleteAllRecord(DWORD startCluster);
void  GetContent(DWORD startCluster, PDWORD size, PDWORD files, PDWORD folders);
void  ClearClusters(DWORD cluster);
void  ReadSector(BYTE *, DWORD sectorIndex);
void  WriteSector(BYTE *, DWORD sectorIndex);
void  GetNextSector(PFile pfile, DWORD curSectorIndex, PDWORD nextSectorIndex, PUINT isLastSector);

void init_fs_fat();

int rw_sector_fat(int io_type, int dev, unsigned long long pos, int bytes, int proc_nr, void *buf);
int rw_sector_sched_fat(int io_type, int dev, int pos, int bytes, int proc_nr, void *buf);

void DisErrorInfo(STATE state);

int sys_CreateFile(void *uesp);
int sys_DeleteFile(void *uesp);
int sys_OpenFile(void *uesp);
int sys_CloseFile(void *uesp);
int sys_WriteFile(void *uesp);
int sys_ReadFile(void *uesp);
int sys_OpenDir(void *uesp);
int sys_CreateDir(void *uesp);
int sys_DeleteDir(void *uesp);
int sys_ListDir(void *uesp);

#endif /* _FAT32_H_ */