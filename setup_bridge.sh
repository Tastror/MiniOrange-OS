#!/bin/sh

# 在这填写网卡名称 
nic="eth0"

# 设置允许转发
iptables -t filter -A FORWARD -i br0 -j ACCEPT
iptables -t filter -A FORWARD -o br0 -j ACCEPT
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
# 设置网桥ip
ifconfig br0 10.0.2.2/24
