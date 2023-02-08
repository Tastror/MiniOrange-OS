#!/bin/sh

# 设置允许转发
iptables -t filter -A FORWARD -i br0 -j ACCEPT
iptables -t filter -A FORWARD -o br0 -j ACCEPT
# 创建网桥
sudo brctl addbr br0
# 创建tap0设备，让当前用户可以使用
sudo tunctl -t tap0 -u `whoami`
# # 清空网卡ip地址
# sudo ip addr flush dev $1
# 将网卡加入网桥 
sudo brctl addif br0 $1
# 将tap0设备加入网桥 
sudo brctl addif br0 tap0
# 启用三个设备
sudo ifconfig $1 up
sudo ifconfig tap0 up
sudo ifconfig br0 up

sudo ifconfig br0 10.0.2.2/24
sudo ifconfig $1 0.0.0.0 promisc
sudo dhclient br0