#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <common/type.h>
#include <define/define.h>

void  kern_net_test(u8 *args);
void  kern_net_arp();
char *kern_net_receive();

#endif