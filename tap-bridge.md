# QEMU 配置 tap 后端

1. 安装工具

   ```bash
   apt install uml-utilities
   apt install bridge-utils
   ```

2. 查看有线网卡

   ```bash
   ifconfig
   ```

3. 建立网桥，在 `setup-bridge.sh` 中填写网卡名称，然后运行脚本

   ```bash
   ./setup-bridge.sh [网卡名称]
   ```
