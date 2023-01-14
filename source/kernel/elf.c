#include <kernel/elf.h>

#include <kernel/fs.h>
#include <kernel/vfs.h>
#include <lib/string.h>

void read_Ehdr(u32 fd, Elf32_Ehdr *File_Ehdr, u32 offset)
{
    do_vlseek(fd, offset, SEEK_SET);                      // modified by mingxuan 2019-5-24
    do_vread(fd, (void *)File_Ehdr, sizeof(Elf32_Ehdr));  // modified by mingxuan 2019-5-24
}

void read_Phdr(u32 fd, Elf32_Phdr *File_Phdr, u32 offset)
{
    do_vlseek(fd, offset, SEEK_SET);                      // modified by mingxuan 2019-5-24
    do_vread(fd, (void *)File_Phdr, sizeof(Elf32_Phdr));  // modified by mingxuan 2019-5-24
}

void read_Shdr(u32 fd, Elf32_Shdr *File_Shdr, u32 offset)
{
    do_vlseek(fd, offset, SEEK_SET);                      // modified by mingxuan 2019-5-24
    do_vread(fd, (void *)File_Shdr, sizeof(Elf32_Shdr));  // modified by mingxuan 2019-5-23
}