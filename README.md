# Bike Computer

基于 SiFli SF32LB52X、RT-Thread 和 LVGL 9 的自行车码表固件。

## 运行模式

- 离线模式：设备使用 GPS 和传感器记录骑行数据。
- 路书导航模式：设备依据已导入路书导航并记录骑行。
- 手机导航模式：手机 APP 下发高德导航指令，设备负责显示。

当前版本已经建立分层目录骨架，并保留导航演示界面、CO5300 显示、CST922
触摸和 SGM41562B 充电芯片初始化。GPS、IMU、文件系统、BLE 和路书算法尚未
启用。

架构说明见 [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)。

## 编译

在正确初始化的 SiFli SDK PowerShell 环境中执行：

```powershell
cd project
scons --board=t-display-sf32 -j4
```

输出固件位于：

```text
project/build_t-display-sf32_hcpu/main.bin
```
