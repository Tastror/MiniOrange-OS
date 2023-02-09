#!/bin/sh

# 设置允许转发
sudo iptables -t filter -A FORWARD -i br0 -j ACCEPT
sudo iptables -t filter -A FORWARD -o br0 -j ACCEPT
# 创建网桥
echo "- creating net bridge"
sudo brctl addbr br0
# 创建tap0设备，让当前用户可以使用
echo "- creating tap0"
sudo tunctl -t tap0 -u `whoami`
# 清空网卡ip地址
sudo ip addr flush dev $1
sudo ip addr flush dev tap0
# 将网卡加入网桥 
echo "- enabling net bridge"
sudo brctl addif br0 $1 
sudo brctl addif br0 tap0
# 启用三个设备
sudo ifconfig $1 promisc up
sudo ifconfig tap0 promisc up
sudo ifconfig br0 promisc up
# 设置ip地址
# serve as gateway
sudo ifconfig br0 10.0.2.1/24
echo "- done" 
# optional
# sudo dhclient br0 