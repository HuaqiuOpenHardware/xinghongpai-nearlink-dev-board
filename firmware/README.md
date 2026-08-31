# Firmware Examples | 固件示例

The `examples/` directory contains OpenHarmony / Hi3863 sample applications from the original `02-程序源码.zip` package.

These examples reference an external OpenHarmony / HiSilicon WS63 SDK tree and are not expected to build standalone from this repository. Keep third-party SDKs outside this repo unless their license explicitly allows redistribution.

这些示例依赖仓库外部的 OpenHarmony / Hi3863（WS63）SDK，不能把本目录当成可独立编译的完整工程。仓库目前没有经过维护者复核的统一 SDK 版本和一键编译命令。

## Before using an example

1. Record the SDK and OpenHarmony version you are using.
2. Read the example's `BUILD.gn`, `CMakeLists.txt` or local README before copying it into an SDK tree.
3. Check GPIO, I2C, ADC, PWM and other pin assignments against your physical board revision.
4. Replace all network credentials with local test values and never commit real passwords or keys.
5. Capture the complete build command and log when reporting a problem.

## Suggested learning order

- Basic OS primitives: `00_thread`, `01_timer`, `03_mutex`, `04_semaphore`, `05_message`
- Board peripherals: `101_gpioled`, `11_aht20`, `13_adclight`, `201_oled`
- Networking: `14_easy_wifi`, `15_tcpclient`, `16_tcpserver`, `17_udpclient`, `18_udpserver`
- Integrated applications: directories in the `100`, `200` and `300` series

Directory names reflect the imported source package and do not imply that every example has been independently tested in this repository.
