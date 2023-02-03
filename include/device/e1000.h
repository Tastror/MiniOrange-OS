#ifndef _ORANGES_E1000_H
#define _ORANGES_E1000_H

#include <common/type.h>
#include <define/define.h>
#include <device/pci.h>
#include <kernlib/mbuf.h>

#define PCI_E1000_VENDER_ID 0x8086
#define PCI_E1000_DEVICE_ID 0x100E

/* 循环队列的长度 */
#define TX_RING_SIZE 16
#define RX_RING_SIZE 16

// manual [E1000 3.3.3]
struct tx_desc {
    uint64_t addr;     // mbuf的地址
    uint16_t length;   // mbuf的大小
    uint8_t  cso;      // no use
    uint8_t  cmd;      // e1000执行的指令
    uint8_t  status;   // 使用状态
    uint8_t  css;      // no use
    uint16_t special;  // no use
};

// manual [E1000 3.2.3]
struct rx_desc {
    uint64_t addr;   /* Address of the descriptor's data buffer */
    uint16_t length; /* Length of data DMAed into data buffer */
    uint16_t csum;   /* Packet checksum */
    uint8_t  status; /* Descriptor status */
    uint8_t  errors; /* Descriptor Errors */
    uint16_t special;
};

/* Register Set
 *
 * RW - register is both readable and writable
 *
 */

/* Registers */
#define E1000_CTL      (0x00000 >> 2) /* Device Control Register - RW */
#define E1000_STATUS   (0x00008 >> 2) /* Device Status - R */
#define E1000_EECD     (0x00010 >> 2) /* EEPROM/Flash Control/Data - RW */
#define E1000_EERD     (0x00014 >> 2) /* EEPROM Read - RW */
#define E1000_FLA      (0x0001C >> 2) /* Flash Access - RW */
#define E1000_CTRL_EXT (0x00018 >> 2) /* Extended Device Control - RW */
#define E1000_MDIC     (0x00020 >> 2) /* MDI Control - RW */
#define E1000_FCAL     (0x00028 >> 2) /* Flow Control Address Low - RW */
#define E1000_FCAH     (0x0002C >> 2) /* Flow Control Address High - RW */
#define E1000_FCT      (0x00030 >> 2) /* Flow Control Type - RW */
#define E1000_VET      (0x00038 >> 2) /* VLAN EtherType - RW */
#define E1000_FCTTV    (0x00170 >> 2) /* Flow Control Transmit Timer Value - RW */
#define E1000_TXCW     (0x00178 >> 2) /* Transmit Configuration Word - RW */
#define E1000_RXCW     (0x00180 >> 2) /* Receive Configuration Word - R */
#define E1000_LEDCTL   (0x00E00 >> 2) /* LED Control - RW */
#define E1000_PBA      (0x01000 >> 2) /* Packet Buffer Allocation - RW */
#define E1000_ICR      (0x000C0 >> 2) /* Interrupt Cause Read - R */
#define E1000_IMS      (0x000D0 >> 2) /* Interrupt Mask Set - RW */
#define E1000_RCTL     (0x00100 >> 2) /* RX Control - RW */
#define E1000_TCTL     (0x00400 >> 2) /* TX Control - RW */
#define E1000_TIPG     (0x00410 >> 2) /* TX Inter-packet gap -RW */
#define E1000_RDBAL    (0x02800 >> 2) /* RX Descriptor Base Address Low - RW */
#define E1000_RDTR     (0x02820 >> 2) /* RX Delay Timer */
#define E1000_RADV     (0x0282C >> 2) /* RX Interrupt Absolute Delay Timer */
#define E1000_RDH      (0x02810 >> 2) /* RX Descriptor Head - RW */
#define E1000_RDT      (0x02818 >> 2) /* RX Descriptor Tail - RW */
#define E1000_RDLEN    (0x02808 >> 2) /* RX Descriptor Length - RW */
#define E1000_RSRPD    (0x02C00 >> 2) /* RX Small Packet Detect Interrupt */
#define E1000_TDBAL    (0x03800 >> 2) /* TX Descriptor Base Address Low - RW */
#define E1000_TDLEN    (0x03808 >> 2) /* TX Descriptor Length - RW */
#define E1000_TDH      (0x03810 >> 2) /* TX Descriptor Head - RW */
#define E1000_TDT      (0x03818 >> 2) /* TX Descripotr Tail - RW */
#define E1000_MTA      (0x05200 >> 2) /* Multicast Table Array - RW Array */
#define E1000_RA       (0x05400 >> 2) /* Receive Address - RW Array */

/* Device Control */
#define E1000_CTL_SLU     0x00000040 /* set link up */
#define E1000_CTL_FRCSPD  0x00000800 /* force speed */
#define E1000_CTL_FRCDPLX 0x00001000 /* force duplex */
#define E1000_CTL_RST     0x00400000 /* full reset */

/* Transmit Control */
#define E1000_TCTL_RST        0x00000001 /* software reset */
#define E1000_TCTL_EN         0x00000002 /* enable tx */
#define E1000_TCTL_BCE        0x00000004 /* busy check enable */
#define E1000_TCTL_PSP        0x00000008 /* pad short packets */
#define E1000_TCTL_CT         0x00000ff0 /* collision threshold */
#define E1000_TCTL_CT_SHIFT   4
#define E1000_TCTL_COLD       0x003ff000 /* collision distance */
#define E1000_TCTL_COLD_SHIFT 12
#define E1000_TCTL_SWXOFF     0x00400000 /* SW Xoff transmission */
#define E1000_TCTL_PBE        0x00800000 /* Packet Burst Enable */
#define E1000_TCTL_RTLC       0x01000000 /* Re-transmit on late collision */
#define E1000_TCTL_NRTU       0x02000000 /* No Re-transmit on underrun */
#define E1000_TCTL_MULR       0x10000000 /* Multiple request support */

/* Receive Control */
#define E1000_RCTL_RST         0x00000001 /* Software reset */
#define E1000_RCTL_EN          0x00000002 /* enable */
#define E1000_RCTL_SBP         0x00000004 /* store bad packet */
#define E1000_RCTL_UPE         0x00000008 /* unicast promiscuous enable */
#define E1000_RCTL_MPE         0x00000010 /* multicast promiscuous enab */
#define E1000_RCTL_LPE         0x00000020 /* long packet enable */
#define E1000_RCTL_LBM_NO      0x00000000 /* no loopback mode */
#define E1000_RCTL_LBM_MAC     0x00000040 /* MAC loopback mode */
#define E1000_RCTL_LBM_SLP     0x00000080 /* serial link loopback mode */
#define E1000_RCTL_LBM_TCVR    0x000000C0 /* tcvr loopback mode */
#define E1000_RCTL_DTYP_MASK   0x00000C00 /* Descriptor type mask */
#define E1000_RCTL_DTYP_PS     0x00000400 /* Packet Split descriptor */
#define E1000_RCTL_RDMTS_HALF  0x00000000 /* rx desc min threshold size */
#define E1000_RCTL_RDMTS_QUAT  0x00000100 /* rx desc min threshold size */
#define E1000_RCTL_RDMTS_EIGTH 0x00000200 /* rx desc min threshold size */
#define E1000_RCTL_MO_SHIFT    12         /* multicast offset shift */
#define E1000_RCTL_MO_0        0x00000000 /* multicast offset 11:0 */
#define E1000_RCTL_MO_1        0x00001000 /* multicast offset 12:1 */
#define E1000_RCTL_MO_2        0x00002000 /* multicast offset 13:2 */
#define E1000_RCTL_MO_3        0x00003000 /* multicast offset 15:4 */
#define E1000_RCTL_MDR         0x00004000 /* multicast desc ring 0 */
#define E1000_RCTL_BAM         0x00008000 /* broadcast enable */
/* these buffer sizes are valid if E1000_RCTL_BSEX is 0 */
#define E1000_RCTL_SZ_2048 0x00000000 /* rx buffer size 2048 */
#define E1000_RCTL_SZ_1024 0x00010000 /* rx buffer size 1024 */
#define E1000_RCTL_SZ_512  0x00020000 /* rx buffer size 512 */
#define E1000_RCTL_SZ_256  0x00030000 /* rx buffer size 256 */
/* these buffer sizes are valid if E1000_RCTL_BSEX is 1 */
#define E1000_RCTL_SZ_16384     0x00010000 /* rx buffer size 16384 */
#define E1000_RCTL_SZ_8192      0x00020000 /* rx buffer size 8192 */
#define E1000_RCTL_SZ_4096      0x00030000 /* rx buffer size 4096 */
#define E1000_RCTL_VFE          0x00040000 /* vlan filter enable */
#define E1000_RCTL_CFIEN        0x00080000 /* canonical form enable */
#define E1000_RCTL_CFI          0x00100000 /* canonical form indicator */
#define E1000_RCTL_DPF          0x00400000 /* discard pause frames */
#define E1000_RCTL_PMCF         0x00800000 /* pass MAC control frames */
#define E1000_RCTL_BSEX         0x02000000 /* Buffer size extension */
#define E1000_RCTL_SECRC        0x04000000 /* Strip Ethernet CRC */
#define E1000_RCTL_FLXBUF_MASK  0x78000000 /* Flexible buffer size */
#define E1000_RCTL_FLXBUF_SHIFT 27         /* Flexible buffer shift */

/* Transmit Descriptor command definitions [E1000 3.3.3.1] */
#define E1000_TXD_CMD_EOP 0x01 /* End of Packet */
#define E1000_TXD_CMD_RS  0x08 /* Report Status */

/* Transmit Descriptor status definitions [E1000 3.3.3.2] */
#define E1000_TXD_STAT_DD 0x00000001 /* Descriptor Done */

/* Receive Descriptor bit definitions [E1000 3.2.3.1] */
#define E1000_RXD_STAT_DD  0x01 /* Descriptor Done */
#define E1000_RXD_STAT_EOP 0x02 /* End of Packet */

extern uint32_t *e1000_regs;

int  pci_e1000_attach(struct pci_func *pcif);
int  e1000_transmit(struct mbuf *m);
void e1000_receive(void);
void e1000_receive_pack_handler();

#endif  // SOL >= 6
