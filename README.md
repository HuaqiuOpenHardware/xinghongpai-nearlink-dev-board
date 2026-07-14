# 星鸿派-星闪开发板

![星鸿派-星闪开发板](assets/cover.png)

星鸿派是一款基于海思 WS63V100 / Hi3863 平台的星闪开发板，支持 Wi-Fi、BLE 与 SLE 通信，面向物联网原型、智能家电、环境监测和嵌入式教学场景。板载 0.96 寸 OLED、6 个按键、复位按键和温湿度模块。

原始项目页：https://p.eda.cn/d-1328625634846441472

## Repository layout

- `hardware/kicad/`: KiCad schematic / PCB / project files.
- `hardware/bom/`: BOM spreadsheet exported from the original project page.
- `firmware/examples/`: OpenHarmony / Hi3863 example code from the original source package.
- `downloads/MANIFEST.md`: Original downloadable package names, source URLs and SHA256 checksums.
- `assets/`: Project images used by this README.

## Hardware

- Main controller: HiSilicon Hi3863 / WS63V100 series.
- Connectivity: Wi-Fi, BLE, SLE / NearLink.
- OS target: OpenHarmony.
- PCB: 96 mm x 70 mm, 2-layer board, KiCad source provided.

The original project page mentions schematic, PCB layout, Gerber, BOM and engineering files. The currently downloadable PCB package contains KiCad project, schematic and PCB files; Gerber output is not included as a separate artifact in this first GitHub import.

## Firmware

The firmware examples are intended to be copied into an OpenHarmony / Hi3863 SDK source tree. Several example README files include per-demo build notes. Start with:

- `firmware/examples/11_aht20/README.md`
- `firmware/examples/13_adclight/README.md`
- `firmware/examples/14_easy_wifi/README.md`

## Downloads and checksums

See `downloads/MANIFEST.md` for original filenames, source URLs and SHA256 checksums.

## License notes

The p.eda.cn project page shows "CERN Open Hardware License" in the license field, while the page content also states Apache 2.0 for commercial reuse. Firmware source files include Apache-2.0 headers from HiHope Open Source Organization and HiSilicon. Because this is a mixed hardware / firmware / documentation repository, see `LICENSES/README.md` before redistributing derivative work.

## Attribution

This repository mirrors and organizes the public materials from the 华秋开源硬件社区 project "星鸿派-星闪开发板" for easier GitHub discovery, issue tracking and collaboration.
