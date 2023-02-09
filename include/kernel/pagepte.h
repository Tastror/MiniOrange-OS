#ifndef _PAGEPTE_H_
#define _PAGEPTE_H_

#include <common/type.h>
#include <define/define.h>

/* core.asm */
u32  read_cr2();            // add by visual 2016.5.9
u32  read_cr3();            // add by network 2023.2.9
void refresh_page_cache();  // add by visual 2016.5.12

/* pagepte.c */
u32 init_page_pte(u32 pid);  // edit by visual 2016.4.28

void page_fault_handler(u32 vec_no, u32 err_code, u32 eip, u32 cs, u32 eflags);  // add by visual 2016.4.19

u32  get_pde_index(u32 AddrLin);  // add by visual 2016.4.28
u32  get_pte_index(u32 AddrLin);
u32  get_pde_phy_addr(u32 pid);
u32  get_pte_phy_addr(u32 pid, u32 AddrLin);
u32  get_page_phy_addr(u32 pid, u32 AddrLin);  // 线性地址
u32  pte_exist(u32 PageTblAddrPhy, u32 AddrLin);
u32  phy_exist(u32 PageTblPhyAddr, u32 AddrLin);
void write_page_pde(u32 PageDirPhyAddr, u32 AddrLin, u32 TblPhyAddr, u32 Attribute);
void write_page_pte(u32 TblPhyAddr, u32 AddrLin, u32 PhyAddr, u32 Attribute);
u32  vmalloc(u32 size);
int  lin_mapping_phy(u32 AddrLin, u32 phy_addr, u32 pid, u32 pde_Attribute, u32 pte_Attribute);  // edit by visual 2016.5.19

void clear_kernel_pagepte_low();  // add by visual 2016.5.12

void mmio_init();
void mmio_recover_pagetable(int pid);

uint32_t *mmio_map_region(uint32_t base, uint32_t size);               // add by netowrk 2023.1.17
pte_t    *pgdir_walk(pde_t *pgdir, const void *va, const int create);  // add by netowrk 2023.1.17

#endif