#include <device/e1000.h>
#include <device/pci.h>
#include <device/pcireg.h>
#include <device/x86.h>
#include <kernlib/assert.h>
#include <kernlib/stdio.h>
#include <kernlib/string.h>

/* Parameters for PCI */
// Flags to do "lspci" at bootup
static uint32_t pci_show_devs = 1;
static uint32_t pci_show_addrs = 0;
// PCI "configuration mechanism one"
static uint32_t pci_conf1_addr_ioport = 0x0cf8;
static uint32_t pci_conf1_data_ioport = 0x0cfc;

/* Declarations */
static int pci_bridge_attach(struct pci_func *pcif);
// int pci_e1000_attach(struct pci_func *pcif);  // kernel/e1000.c

// PCI driver table
struct pci_driver {
    uint32_t key1, key2;
    int (*attachfn)(struct pci_func *pcif);
};

// pci_attach_class matches the class and subclass of a PCI device
struct pci_driver pci_attach_class[] = {
    {PCI_CLASS_BRIDGE, PCI_SUBCLASS_BRIDGE_PCI, &pci_bridge_attach},
    {0, 0, 0},
};

// pci_attach_vendor matches the vendor ID and device ID of a PCI device. key1
// and key2 should be the vendor ID and device ID respectively
// struct pci_driver pci_attach_vendor[] = {
//     {PCI_E1000_VENDER_ID, PCI_E1000_DEVICE_ID, &pci_e1000_attach},
//     {0, 0, 0},
// };

// 本函数为 "outl pci_addr_ioport, <bus, dev, func, offset>"
// 用于切换当前读写的地址
static void pci_conf1_set_addr(
    uint32_t bus,
    uint32_t dev,
    uint32_t func,
    uint32_t offset
)
{
    assert(bus < 256);
    assert(dev < 32);
    assert(func < 8);
    assert(offset < 256);
    assert((offset & 0x3) == 0);

    // 对 pci 总线进行读写，需要用  "inl port, data" / "outl port, data" 指令
    // "inl" 是读取，"outl" 是写入，"port" 是读写的端口，"data" 是读写的数据
    //
    // port 是固定的，可以写死，有两个，为 0x0cf8 (地址读写) 和 0x0cfc (数据读写)
    // 地址读写用来切换地址；数据读写用来读写真实信息
    // 读取方法：outl 0x0cf8 (地址读写) 写入地址，然后 inl 0x0cfc (数据读写) 就能获得地址上的信息
    // 写入方法：outl 0x0cf8 (地址读写) 写入地址，然后 outl 0x0cfc (数据读写) 写入地址上的信息
    //
    // 数据读写就是纯粹数据了；
    // 地址读写的协议中，data 所需要填写的内容为
    //
    //   31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
    //    1  (     nothing     )  [        bus         ]  [    dev    ]  [funct]  [      reg     ]  0  0
    // enable     reseread_bar_dataed              bus number         dev number  function num    reg num
    //
    // 前面是总线、设备、功能的编号，都好理解，但是 reg num 是什么呢？
    // 答案是 16 个不同的寄存器，分别保存了如下信息（着重看第一个、最后一个和 04 ~ 09）
    //
    // reg 00:  device ID 16bit, vender ID 16bit
    // reg 01:  statuc 16bit, command 16bit
    // reg 02:  class code 24bit, revision ID 8bit
    // reg 03:  bist, header type, lat timer, cache line. each 8bit
    // reg 04 ~ reg 09:  bar register * 6, 32bit [也就是用于设备的具体功能的 6 个读写寄存器]
    // reg 10:  cardbus cis pointer 32bit
    // reg 11:  subsystem ID 16bit, subsystem vender ID 16bit
    // reg 12:  expansion rom base address 32bit
    // reg 13:  nothing 24bit, cap pointer 8bit
    // reg 14:  nothing 32bit
    // reg 15:  max lat, min gnt, interrupt pin, interrupt line. each 8bit

    u32 target_address = (1 << 31) | (bus << 16) | (dev << 11) | (func << 8) | (offset);
    outl(pci_conf1_addr_ioport, target_address);
}

static int
// __attribute__((warn_unused_result))
pci_attach_match(
    uint32_t key1, uint32_t key2,
    struct pci_driver *list, struct pci_func *pcif
)
{
    uint32_t i;

    for (i = 0; list[i].attachfn; i++) {
        if (list[i].key1 == key1 && list[i].key2 == key2) {
            int r = list[i].attachfn(pcif);
            if (r > 0)
                return r;
            if (r < 0)
                kprintf(
                    "pci_attach_match: attaching "
                    "%x.%x (%p): e\n",
                    key1, key2, list[i].attachfn, r
                );
        }
    }
    return 0;
}

// static int pci_attach(struct pci_func *f)
// {
//     return pci_attach_match(PCI_CLASS(f->dev_class), PCI_SUBCLASS(f->dev_class), &pci_attach_class[0], f) ||
//            pci_attach_match(PCI_VENDOR(f->dev_id), PCI_PRODUCT(f->dev_id), &pci_attach_vendor[0], f);
// }

static const char *pci_class[] = {
    [0x0] = "Unknown",
    [0x1] = "Storage controller",
    [0x2] = "Network controller",
    [0x3] = "Display controller",
    [0x4] = "Multimedia device",
    [0x5] = "Memory controller",
    [0x6] = "Bridge device",
};

static void pci_print_func(struct pci_func *f)
{
    const char *class = pci_class[0];
    if (PCI_CLASS(f->dev_class) < ARRAY_SIZE(pci_class))
        class = pci_class[PCI_CLASS(f->dev_class)];

    kprintf("PCI: %02x:%02x.%d: %04x:%04x: class: %x.%x (%s) irq: %d\n", f->bus->busno, f->dev, f->func, PCI_VENDOR(f->dev_id), PCI_PRODUCT(f->dev_id), PCI_CLASS(f->dev_class), PCI_SUBCLASS(f->dev_class), class, f->irq_line);
}

// f 为设备基础信息；off = 寄存器编号 << 2，即寄存器字节编号（宏定义的已经 << 2 了）
static uint32_t pci_conf_read(struct pci_func *f, uint32_t off)
{
    pci_conf1_set_addr(f->bus->busno, f->dev, f->func, off);
    return inl(pci_conf1_data_ioport);
}

// f 为设备基础信息；off = 寄存器编号 << 2，即寄存器字节编号（宏定义的已经 << 2 了）
static void pci_conf_write(struct pci_func *f, uint32_t off, uint32_t write_data)
{
    pci_conf1_set_addr(f->bus->busno, f->dev, f->func, off);
    outl(pci_conf1_data_ioport, write_data);
}

static int pci_scan_bus(struct pci_bus *bus)
{
    int32_t         totaldev = 0;  // total count of device
    struct pci_func rf;            // root node of bus
    memset(&rf, 0, sizeof(rf));
    rf.bus = bus;

    for (rf.dev = 0; rf.dev < 32; rf.dev++) {
        // BIST/Header Type/Latency Timer/Cache Line Size Register
        uint32_t bhlcr = pci_conf_read(&rf, PCI_BHLC_REG);
        if (PCI_HDRTYPE_TYPE(bhlcr) > 1)  // Unsupported or No device
            continue;

        totaldev++;

        struct pci_func srf = rf;  // son of root node
        for (srf.func = 0; srf.func < (PCI_HDRTYPE_MULTIFN(bhlcr) ? 8 : 1); srf.func++) {
            struct pci_func ssrf = srf;  // son of son of root note

            ssrf.dev_id = pci_conf_read(&srf, PCI_ID_REG);
            // vendor is the inventor of the device,
            // like Intel's vendor is 0x8086,
            // and 0xffff means no device.
            if (PCI_VENDOR(ssrf.dev_id) == 0xffff)
                continue;

            uint32_t intr = pci_conf_read(&ssrf, PCI_INTERRUPT_REG);
            ssrf.irq_line = PCI_INTERRUPT_LINE(intr);
            ssrf.dev_class = pci_conf_read(&ssrf, PCI_CLASS_REG);
            if (pci_show_devs)
                pci_print_func(&ssrf);
            // pci_attach(&ssrf);

            if (
                PCI_VENDOR(ssrf.dev_id) == PCI_E1000_VENDER_ID &&
                PCI_PRODUCT(ssrf.dev_id) == PCI_E1000_DEVICE_ID
            ) {
                // kprintf("Found e1000\n");
                pci_e1000_attach(&ssrf);
            }
        }
    }

    return totaldev;
}

void pci_func_enable(struct pci_func *f)
{
    pci_conf_write(f, PCI_COMMAND_STATUS_REG, PCI_COMMAND_IO_ENABLE | PCI_COMMAND_MEM_ENABLE | PCI_COMMAND_MASTER_ENABLE);

    uint32_t bar_width = 4;

    // 04 ~ 09 的 6 个 bar 寄存器的初始化
    for (
        uint32_t bar = PCI_MAPREG_START; bar < PCI_MAPREG_END;
        bar += bar_width
    ) {
        uint32_t old_bar_data = pci_conf_read(f, bar);

        bar_width = 4;
        pci_conf_write(f, bar, 0xffffffff);
        uint32_t read_bar_data = pci_conf_read(f, bar);
        if (read_bar_data == 0)
            continue;

        int regnum = PCI_MAPREG_NUM(bar);

        uint32_t base, size;
        if (PCI_MAPREG_TYPE(read_bar_data) == PCI_MAPREG_TYPE_MEM) {
            if (PCI_MAPREG_MEM_TYPE(read_bar_data) == PCI_MAPREG_MEM_TYPE_64BIT)
                bar_width = 8;
            size = PCI_MAPREG_MEM_SIZE(read_bar_data);
            base = PCI_MAPREG_MEM_ADDR(old_bar_data);
            if (pci_show_addrs)
                kprintf("  mem region %d: %d bytes at 0x%x\n", regnum, size, base);
        } else {
            size = PCI_MAPREG_IO_SIZE(read_bar_data);
            base = PCI_MAPREG_IO_ADDR(old_bar_data);
            if (pci_show_addrs)
                kprintf("  io region %d: %d bytes at 0x%x\n", regnum, size, base);
        }

        pci_conf_write(f, bar, old_bar_data);
        f->reg_base[regnum] = base;
        f->reg_size[regnum] = size;

        if (size && !base)
            kprintf(
                "PCI device %02x:%02x.%d (%04x:%04x) "
                "may be misconfigured: "
                "region %d: base 0x%x, size %d\n",
                f->bus->busno, f->dev, f->func,
                PCI_VENDOR(f->dev_id), PCI_PRODUCT(f->dev_id),
                regnum, base, size
            );
    }

    kprintf(
        "PCI function %02x:%02x.%d (%04x:%04x) enabled\n",
        f->bus->busno, f->dev, f->func,
        PCI_VENDOR(f->dev_id), PCI_PRODUCT(f->dev_id)
    );
}

int init_pci(void)
{
    static struct pci_bus root_bus;
    memset(&root_bus, 0, sizeof(root_bus));

    return pci_scan_bus(&root_bus);
}

static int pci_bridge_attach(struct pci_func *pcif)
{
    uint32_t ioreg = pci_conf_read(pcif, PCI_BRIDGE_STATIO_REG);
    uint32_t busreg = pci_conf_read(pcif, PCI_BRIDGE_BUS_REG);

    if (PCI_BRIDGE_IO_32BITS(ioreg)) {
        kprintf("PCI: %02x:%02x.%d: 32-bit bridge IO not supported.\n", pcif->bus->busno, pcif->dev, pcif->func);
        return 0;
    }

    struct pci_bus nbus;
    memset(&nbus, 0, sizeof(nbus));
    nbus.parent_bridge = pcif;
    nbus.busno = (busreg >> PCI_BRIDGE_BUS_SECONDARY_SHIFT) & 0xff;

    if (pci_show_devs)
        kprintf("PCI: %02x:%02x.%d: bridge to PCI bus %d--%d\n", pcif->bus->busno, pcif->dev, pcif->func, nbus.busno, (busreg >> PCI_BRIDGE_BUS_SUBORDINATE_SHIFT) & 0xff);

    pci_scan_bus(&nbus);
    return 1;
}