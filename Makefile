# make 主文件


# OBJDIR 用于存放编译出来的可重定位文件
OBJDIR := obj
# INCDIR 用于存放各种头文件 (*.h)
INCDIR := include
# SRCDIR 用于存放 .c .asm 文件
SRCDIR := source
# IMGDIR 用于生成基本的 a.img 文件
IMGDIR := img_gen

# 编译以及日常工具
CC := gcc
# 汇编器
AS := nasm
# 静态库编辑器
AR := ar
# 链接器
LD := ld
# 复制文件
OBJCOPY := objcopy
# 反编译
OBJDUMP := objdump
# 查询可重定位文件符号表
NM := nm

DEFS := 

# gcc 的相关命令参数
# $(DEFS) 定义一些可能的参数
# -O0 0优化，保证程序按照代码语义走而不被优化，方便调试
# -fno-builtin 静止使用 gcc 内置函数，具体查手册
CFLAGS := $(CFLAGS) $(DEFS) -O0 -fno-builtin
# -I 编译时去指定文件夹查找头文件
# -MD 一个黑科技暂时可以不需要了解，总之是在头文件依赖变动的时候能够及时更新 target
CFLAGS += -I $(INCDIR) -MD
# -fno-stack-protector 禁止栈保护（金丝雀保护机制，内核代码扛不住）
CFLAGS += -fno-stack-protector
# -std=gnu99 规定编译的语言规范为 gnu99
CFLAGS += -std=gnu99
# -fno-pie 不创建动态链接库
CFLAGS += -fno-pie
# -static 编译静态程序
# -m32 编译 32 位程序
CFLAGS += -static -m32
# -g 打开 gdb 调试信息，能够允许 gdb 的时候调试
CFLAGS += -g
# 一车的 warning，在编译的时候可能会很有用
CFLAGS += -Wall -Wno-format -Wno-unused -Werror

# ld 链接器的相关命令参数
# -m elf_i386 链接的格式为 i386
LDFLAGS := -m elf_i386
# -nostdlib 不链接 gcc 的标准库，用库只能用命令行的
LDFLAGS += -nostdlib
# 获取 gcc 的库文件（除法取模会用到）
GCC_LIB := $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

# 记录每个 OBJDIR 里存放的每个子文件夹
OBJDIRS :=

# FAT32镜像文件
IMAGE = $(OBJDIR)/a.img

# 用来生成该镜像文件的脚本和 c 语言
BUILD_IMAGE_SCRIPT = $(IMGDIR)/build_img.sh
GENERATE_IMAGE_C = $(IMGDIR)/generate_sparseness.c

# added by mingxuan 2020-9-12
# Offset of os_boot in hd
# 活动分区所在的扇区号
# OSBOOT_SEC = 4096
# 活动分区所在的扇区号对应的字节数
# OSBOOT_OFFSET = $(OSBOOT_SEC)*512 
OSBOOT_OFFSET = 1048576
# FAT32 规范规定 os_boot 的前 89 个字节是 FAT32 的配置信息
# OSBOOT_START_OFFSET = OSBOOT_OFFSET + 90
OSBOOT_START_OFFSET = 1048666  # for test12.img

# added by mingxuan 2020-10-29
# Offset of fs in hd
# 文件系统标志所在扇区号 = 文件系统所在分区的第1个扇区 + 1
# ORANGE_FS_SEC = 6144 + 1 = 6145
# 文件系统标志所在扇区 = $(ORANGE_FS_SEC)*512
ORANGE_FS_START_OFFSET = 3146240
# FAT32_FS_SEC = 53248 + 1 = 53249
# 文件系统标志所在扇区 = $(ORANGE_FS_SEC)*512
FAT32_FS_START_OFFSET = 27263488

# oranges 文件系统在硬盘上的起始扇区
# PART_START_SECTOR = 92049
PART_START_SECTOR = 6144  # modified by mingxuan 2020-10-12

# 写入硬盘的起始位置
# INSTALL_PHY_SECTOR = PART_START_SECTOR + 951 # Why is 951 ?
INSTALL_PHY_SECTOR = 7095  # modified by mingxuan 2020-10-12
# assert(INSTALL_PHY_SECTOR > PART_START_SECTOR)

# 写入硬盘的文件大小
INSTALL_NR_SECTORS = 1000

INSTALL_START_SECTOR = $(shell echo $$(($(INSTALL_PHY_SECTOR)-$(PART_START_SECTOR))))
SUPER_BLOCK_ADDR = $(shell echo $$((($(PART_START_SECTOR)+1)*512)))

INSTALL_TYPE = INSTALL_TAR

INSTALL_FILENAME = app.tar

# 网络相关内容
QEMUOPTS :=
# 使用 SLIRP 后端
QEMUOPTS += -netdev user,id=mynet0,hostfwd=tcp::5555-:22,
# 设置虚拟机使用的网卡 e1000
QEMUOPTS += -device e1000,netdev=mynet0
# 设置网络通信监听文件存储
QEMUOPTS += -object filter-dump,id=myfile1,netdev=mynet0,file=dump.pcap


# 第一个命令。如果你想要仅编译，换成 all 就好
all:

# 加载 $(SRCDIR) 中的所有编译命令
include $(SRCDIR)/Makefrag

.DELETE_ON_ERROR:

# xv6 黑科技，获取编译命令，如果命令较新则重新编译所有文件
.PRECIOUS: $(OBJDIR)/.vars.% \
		$(OBJDIR)/boot/%.o $(OBJDIR)/boot/%.d \
		$(OBJDIR)/fs_flags/%.o $(OBJDIR)/fs_flags/%.d \
		$(OBJDIR)/kernlib/%.o $(OBJDIR)/kernlib/%.d \
		$(OBJDIR)/device/%.o $(OBJDIR)/device/%.d \
		$(OBJDIR)/kernel/%.o $(OBJDIR)/kernel/%.d \
		$(OBJDIR)/userlib/%.o $(OBJDIR)/userlib/%.d \
		$(OBJDIR)/user/%.o $(OBJDIR)/user/%.d \

$(OBJDIR)/.vars.%: FORCE
	@echo "$($*)" | cmp -s $@ || echo "$($*)" > $@

$(IMAGE): $(OBJDIR)/boot/mbr.bin \
		$(OBJDIR)/boot/boot.bin \
		$(OBJDIR)/boot/loader.bin \
		$(OBJDIR)/kernel/kernel.bin \
		$(OBJDIR)/user/$(USER_TAR) \
		$(BUILD_IMAGE_SCRIPT) \
		$(GENERATE_IMAGE_C) \
		$(FS_FLAG_OBJFILES)
	@sudo bash $(BUILD_IMAGE_SCRIPT) $@ $(OBJDIR) $(GENERATE_IMAGE_C) $(OSBOOT_START_OFFSET)
	@sudo dd if=$(OBJDIR)/user/$(USER_TAR) of=$@ bs=512 count=$(INSTALL_NR_SECTORS) seek=$(INSTALL_PHY_SECTOR) conv=notrunc
	@sudo dd if=$(OBJDIR)/fs_flags/orange_flag.bin of=$@ bs=1 count=1 seek=$(ORANGE_FS_START_OFFSET) conv=notrunc
	@sudo dd if=$(OBJDIR)/fs_flags/fat32_flag.bin of=$@ bs=1 count=11 seek=$(FAT32_FS_START_OFFSET) conv=notrunc
	@sudo chmod 777 $(IMAGE)

.PHONY: FORCE all run gdb gdb-no-graphic monitor disassemble clean


all: $(IMAGE)

run: $(IMAGE)
	@qemu-system-i386 \
	-boot order=a \
	-drive file=$<,format=raw \
	$(QEMUOPTS)

gdb: $(IMAGE)
	@qemu-system-i386 \
	-boot order=a \
	-drive file=$<,format=raw \
	-s -S \
	$(QEMUOPTS)

gdb-no-graphic: $(IMAGE)
	@qemu-system-i386 \
	-nographic \
	-boot order=a \
	-drive file=$<,format=raw \
	-s -S \
	$(QEMUOPTS)

# 调试的内核代码 elf
KERNDBG := $(OBJDIR)/kernel/kernel.dbg

monitor: $(IMAGE)
	@gdb \
	-ex 'set confirm off' \
	-ex 'target remote localhost:1234' \
	-ex 'file $(KERNDBG)'			

disassemble: $(IMAGE)
	@$(OBJDUMP) -S $(KERNDBG) | less

# 黑科技时间，获取每个 .c 对应的头文件依赖
# 挺难整明白的，不建议一开始整明白，反正从 xv6 上抄的，不明觉厉
$(OBJDIR)/.deps: $(foreach dir, $(OBJDIRS), $(wildcard $(OBJDIR)/$(dir)/*.d))
	@mkdir -p $(@D)
	@perl mergedep.pl $@ $^

-include $(OBJDIR)/.deps

clean:
	@rm -rf $(OBJDIR)