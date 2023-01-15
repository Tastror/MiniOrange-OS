#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <common/type.h>
#include <define/define.h>

extern u8* e1000_save_buff[E1000_PACK_BUFF_SAVE_SIZE];
extern u16 e1000_save_len;

void kern_net_test(u8 *args);
void e1000_send_pack_handler();
void e1000_receive_pack_handler();

#endif