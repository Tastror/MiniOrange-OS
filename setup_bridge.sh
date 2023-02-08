#!/bin/sh

# 在这填写网卡名称 
nic="eth0"

# 创建网桥
brctl addbr br0
# 清空网卡ip地址
ip addr flush dev ${nic}
# 将网卡加入网桥 
brctl addif br0 ${nic}
# 创建tap0设备，让当前用户可以使用
tunctl -t tap0 -u `whoami`
# 将tap0设备加入网桥 
brctl addif br0 tap0
# 启用三个设备
ifconfig ${nic} up
ifconfig tap0 up
ifconfig br0 up