<h1 align="center">Xinghongpai NearLink Dev Board</h1>

<p align="center"><strong>星鸿派 · WS63V100 / Hi3863 星闪开源开发板</strong></p>

<p align="center">
  <a href="README_EN.md">English</a> |
  <strong>简体中文</strong>
</p>

<p align="center">
  <a href="https://p.eda.cn/d-1328625634846441472">原始项目</a> ·
  <a href="hardware/">硬件设计</a> ·
  <a href="hardware/bom/">BOM</a> ·
  <a href="firmware/">固件示例</a> ·
  <a href="downloads/MANIFEST.md">资料校验</a>
</p>

<p align="center">
  🛠️ <a href="https://github.com/HuaqiuOpenHardware/xinghongpai-nearlink-dev-board/issues/new/choose">提交问题</a>
  &nbsp;·&nbsp;
  🤝 <a href="CONTRIBUTING.md">参与贡献</a>
  &nbsp;·&nbsp;
  ⭐ <a href="https://github.com/HuaqiuOpenHardware/xinghongpai-nearlink-dev-board">收藏项目</a>
</p>

![星鸿派 WS63V100 星闪开源开发板](assets/cover.png)

星鸿派是一款基于海思 **WS63V100 / Hi3863** 平台的开源开发板，支持 **星闪 NearLink（SLE）**、Wi-Fi 和 BLE，面向 OpenHarmony 学习、物联网原型、智能家电、环境监测与嵌入式教学。本仓库直接开放 KiCad 原理图与 PCB、BOM，以及 OpenHarmony / Hi3863 固件示例。

> 本仓库整理自华秋开源硬件社区公开项目。硬件许可证的具体 CERN-OHL 版本尚待项目方确认；复刻、修改或商用前请先阅读[许可证说明](#许可证说明)。

## 一眼看懂

| 项目 | 内容 |
| --- | --- |
| 主控平台 | HiSilicon WS63V100 / Hi3863 系列 |
| 无线连接 | NearLink SLE、Wi-Fi、BLE |
| 软件方向 | OpenHarmony / Hi3863 示例工程 |
| 板载资源 | 0.96 寸 OLED、6 个用户按键、复位按键、温湿度模块 |
| 硬件资产 | KiCad 工程、原理图、PCB、BOM |
| PCB 信息 | 约 96 mm × 70 mm，双层板（来自原项目页） |
| 适用人群 | 嵌入式开发者、创客、学生、教师与硬件研究者 |

## 开放资料

| 想找什么 | 仓库入口 | 当前状态 |
| --- | --- | --- |
| 原理图与 PCB 源文件 | [`hardware/kicad/`](hardware/kicad/) | 已提交，可直接查看与下载 |
| BOM 物料清单 | [`hardware/bom/`](hardware/bom/) | 已提交 XLSX |
| OpenHarmony / Hi3863 示例 | [`firmware/examples/`](firmware/examples/) | 已提交；依赖外部 SDK，不是独立工程 |
| 硬件与固件使用边界 | [`hardware/README.md`](hardware/README.md)、[`firmware/README.md`](firmware/README.md) | 已说明 |
| 原始资料来源与校验值 | [`downloads/MANIFEST.md`](downloads/MANIFEST.md) | 已记录 URL、大小和 SHA256 |
| 项目事实与来源 | [`PROJECT_FACTSHEET.md`](PROJECT_FACTSHEET.md) | 已整理 |
| 导入时的改动 | [`CHANGES_FROM_ORIGINAL.md`](CHANGES_FROM_ORIGINAL.md) | 已记录 |

原项目页提到 Gerber 文件，但首次导入使用的 PCB 下载包中没有独立 Gerber 制造文件。本仓库不把“页面提到”表述成“仓库已经提供”。

## 快速开始

### 查看或修改硬件

1. 打开 [`hardware/kicad/`](hardware/kicad/) 并下载 `.kicad_pro`、`.kicad_sch` 与 `.kicad_pcb` 文件。
2. 使用 KiCad 打开工程；复刻前同时核对原理图、PCB、[`BOM`](hardware/bom/) 和板卡实物版本。
3. 生成 Gerber、钻孔文件或贴片文件前，重新执行 ERC/DRC，并人工确认封装、替代料、电源与射频设计。

### 使用固件示例

1. 先阅读 [`firmware/README.md`](firmware/README.md)，确认示例并非可独立编译的完整 SDK。
2. 准备与示例匹配的 OpenHarmony / Hi3863（WS63）SDK 环境。
3. 从 GPIO、定时器、AHT20、OLED 等基础示例开始，再进入 Wi-Fi、TCP/UDP 与综合应用示例。
4. 提交问题时请提供 SDK / OpenHarmony 版本、示例目录、完整编译日志和板卡版本。

> 仓库尚未提供经过维护者复核的统一编译命令，因此这里不编造“一键编译”步骤。欢迎通过 Issue 或 Pull Request 补充可复现的环境说明。

## 固件示例方向

仓库中已有线程、定时器、互斥锁、信号量、消息队列、GPIO、AHT20、ADC、OLED、Wi-Fi、TCP/UDP，以及风扇、火焰检测、交通灯、继电器、温湿度等示例。具体文件以 [`firmware/examples/`](firmware/examples/) 为准。

推荐起点：

- [`11_aht20`](firmware/examples/11_aht20/)：AHT20 温湿度传感器
- [`13_adclight`](firmware/examples/13_adclight/)：ADC 光线采集
- [`14_easy_wifi`](firmware/examples/14_easy_wifi/)：Wi-Fi 连接与热点
- [`201_oled`](firmware/examples/201_oled/)：OLED 显示

## 仓库结构

```text
.
├── .github/                 # Issue 与 Pull Request 模板
├── assets/                 # 封面与实物图片
├── downloads/              # 原始下载地址、文件大小与 SHA256
├── firmware/
│   └── examples/           # OpenHarmony / Hi3863 固件示例
├── hardware/
│   ├── bom/                # BOM 物料清单
│   └── kicad/              # KiCad 原理图与 PCB 工程
├── LICENSES/               # 混合内容的许可证说明
├── CONTRIBUTING.md
├── PROJECT_FACTSHEET.md
├── README.md
└── README_EN.md
```

## 许可证说明

这个仓库包含硬件、固件和第三方资料，不能用一个未经确认的许可证覆盖全部内容：

- 原项目页的许可证字段显示 **CERN Open Hardware License**，但没有注明 CERN-OHL-S、W 或 P 等具体版本。
- 原项目页正文另有 Apache 2.0 与商用表述，两处口径并不完全一致。
- 多个固件文件保留了 HiHope Open Source Organization、HiSilicon 等来源的 Apache-2.0 版权头。
- 芯片文档、SDK、工具和原始压缩包可能适用各自的上游条款。

复刻、再分发、批量制造或商业化前，请阅读 [`LICENSE`](LICENSE) 与 [`LICENSES/README.md`](LICENSES/README.md)，并向项目方确认硬件许可证的准确版本。不要删除文件内已有的版权与许可证声明。

## 参与项目

- 硬件问题：使用 Hardware Issue 模板，注明板卡版本、测量结果、照片与复现步骤。
- 固件问题：使用 Firmware Issue 模板，注明 SDK / OpenHarmony 版本和完整日志。
- 提交修改：先阅读 [`CONTRIBUTING.md`](CONTRIBUTING.md)，并说明 BOM、硬件、固件和许可证影响。
- 安全问题：不要在公开 Issue 中提交 Wi-Fi 密码、密钥或生产设备凭据，详见 [`SECURITY.md`](SECURITY.md)。

## 来源与搜索别名

原始项目：华秋开源硬件社区「[星鸿派-星闪开发板](https://p.eda.cn/d-1328625634846441472)」。

相关名称：星闪、NearLink、SLE、SparkLink、WS63、WS63V100、Hi3863、HiSilicon、OpenHarmony、open-source hardware、development board、schematic、PCB、BOM、KiCad。

---

## 获取帮助与加入交流

如果你正在查找星闪开发资料、搭建 OpenHarmony / Hi3863 环境、复刻硬件，或希望交流项目使用与二次开发，可以添加 **华秋开源硬件小助手**。

添加后可获取：

- 星鸿派项目资料导航与更新提醒
- 开源硬件交流群入群方式
- 社区活动、试用与共创信息
- Issue 提交和项目反馈指引

<p align="center">
  <img src="assets/wechat-assistant-qr.png" width="220" alt="华秋开源硬件小助手微信二维码">
</p>

<p align="center"><strong>微信扫码添加小助手</strong></p>

<p align="center">添加时建议备注“星鸿派”，方便更快对接相关资料与交流群。</p>
