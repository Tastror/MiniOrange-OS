/**
 * elf.h
 * 
 * visual
 * 2016.5.16
 */

#ifndef _ELF_H_
#define _ELF_H_

#include <type.h>

#define EI_NIDENT 16

/**
 * elf 头
 */
typedef struct {
    u8  e_ident[EI_NIDENT];  // ELF 魔数，ELF 字长，字节序，ELF 文件版本等
    u16 e_type;              // ELF 文件类型，REL，可执行文件，共享目标文件等
    u16 e_machine;           // ELF 的 CPU 平台属性
    u32 e_version;           // ELF 版本号
    u32 e_entry;             // ELF 程序的入口虚拟地址
    u32 e_phoff;             // program header table（program 头）在文件中的偏移
    u32 e_shoff;             // section header table（section 头）在文件中的偏移
    u32 e_flags;             // 用于标识 ELF 文件平台相关的属性
    u16 e_ehsize;            // elf header（本文件头）的长度
    u16 e_phentsize;         // program header table 中每一个条目的长度
    u16 e_phnum;             // program header table 中有多少个条目
    u16 e_shentsize;         // section header table 中每一个条目的长度
    u16 e_shnum;             // section header table 中有多少个条目
    u16 e_shstrndx;          // section header table 中字符索引
} Elf32_Ehdr;

/**
 * program 头（程序头）
 */
typedef struct {
    u32 p_type;    // 该 program 类型
    u32 p_offset;  // 该 program 在文件中的偏移量
    u32 p_vaddr;   // 该 program 应该放在这个线性地址
    u32 p_paddr;   // 该 program 应该放在这个物理地址（对只使用物理地址的系统有效）
    u32 p_filesz;  // 该 program 在文件中的长度
    u32 p_memsz;   // 该 program 在内存中的长度（不一定和 filesz 相等）
    u32 p_flags;   // 该 program 读写权限
    u32 p_align;   // 该 program 对齐方式
} Elf32_Phdr;

/**
 * section头（段头）
 */
typedef struct {
    u32 s_name;       // 该 section 段的名字
    u32 s_type;       // 该 section 的类型，代码段，数据段，符号表等
    u32 s_flags;      // 该 section 在进程虚拟地址空间中的属性
    u32 s_addr;       // 该 section 的虚拟地址
    u32 s_offset;     // 该 section 在文件中的偏移
    u32 s_size;       // 该 section 的长度
    u32 s_link;       // 该 section 头部表符号链接
    u32 s_info;       // 该 section 附加信息
    u32 s_addralign;  // 该 section 对齐方式
    u32 s_entsize;    // 该 section 若有固定项目，则给出固定项目的大小，如符号表
} Elf32_Shdr;

void read_Ehdr(u32 fd, Elf32_Ehdr *File_Ehdr, u32 offset);
void read_Phdr(u32 fd, Elf32_Phdr *File_Phdr, u32 offset);
void read_Shdr(u32 fd, Elf32_Shdr *File_Shdr, u32 offset);

#endif /* _ELF_H_ */