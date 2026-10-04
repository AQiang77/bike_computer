# Bike Computer

Bike computer firmware based on SiFli SF32LB52X, RT-Thread, and LVGL 9.

## Operating modes

- Offline mode records rides using the device GPS and sensors.
- Route mode follows an imported route while recording the ride.
- Phone navigation mode displays navigation instructions sent by a mobile app.

The current version provides the compile-ready application structure while
preserving the navigation demo, CO5300 display, CST922 touch controller, and
SGM41562B charger initialization. GPS, IMU, filesystem, BLE, and route matching
are not enabled yet.

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for the module boundaries.

## Build

Run the following in an initialized SiFli SDK PowerShell environment:

```powershell
cd project
scons --board=t-display-sf32 -j4
```

The firmware is generated at:

```text
project/build_t-display-sf32_hcpu/main.bin
```
