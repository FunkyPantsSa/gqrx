# Gqrx

[English](README.md) · 简体中文

[![CI](https://github.com/gqrx-sdr/gqrx/workflows/CI/badge.svg)](https://github.com/gqrx-sdr/gqrx/actions?query=workflow%3ACI+branch%3Amaster)
[![Build](https://github.com/gqrx-sdr/gqrx/workflows/Build/badge.svg)](https://github.com/gqrx-sdr/gqrx/actions?query=workflow%3ABuild+branch%3Amaster)

Gqrx 是一款开源软件定义无线电（SDR）接收机，基于 [GNU Radio](https://gnuradio.org) 和 [Qt 图形界面工具包](https://www.qt.io/) 开发。它可在 Linux 和 Mac 上运行，支持 gr-osmosdr 兼容的硬件，包括 Funcube Dongle、RTL-SDR、Airspy、HackRF、BladeRF、RFSpace、USRP 及通过 SoapySDR 接入的设备。

Gqrx 可作为 AM（调幅）、FM（调频）和 SSB（单边带）接收机并输出音频，也可仅用于 FFT（快速傅里叶变换）频谱分析。它还提供网络套接字接口，以便与外部应用交互。

![Gqrx 主窗口](resources/screenshots/gqrx-main.png)

## 下载

Gqrx 提供源代码包，以及 Linux 和 Mac 二进制发行版。许多 Linux 发行版的软件仓库中也提供 Gqrx。Mac 用户还可通过 [MacPorts](https://ports.macports.org/port/gqrx/summary) 或 [Homebrew](https://formulae.brew.sh/cask/gqrx) 安装。Windows 用户可通过 [radioconda](https://github.com/ryanvolz/radioconda#radioconda) 使用 Gqrx。

- [正式发布版本](https://github.com/gqrx-sdr/gqrx/releases)
- [预发布构建](https://github.com/gqrx-sdr/gqrx/actions?query=workflow%3ABuild+branch%3Amaster)

## 使用

强烈建议先运行 `volk_profile`，再运行 Gqrx。此工具会检测并启用适合当前处理器的优化，在许多情况下可显著提高性能。

首次启动 Gqrx 时会打开设备配置对话框。程序会自动发现已连接的受支持设备，您可在下拉列表中选择要使用的设备。

如果下拉列表中没有您的设备，可能的原因包括：

- 二进制发行版未包含该设备的驱动程序；
- udev 规则未正确配置；
- Linux 内核驱动占用了该设备，导致应用无法访问。

您可以使用设备专用工具进行测试，例如 `rtl_test`、`airspy_rx`、`hackrf_transfer`、`qthid` 等。

Gqrx 支持多套配置和会话，适用于多设备场景，也适用于同一设备采用不同配置的场景。您可通过图形界面加载配置，或使用 `-c` 命令行参数指定配置。运行 `gqrx --help` 可查看完整的命令行参数列表。

教程和操作指南发布在项目网站：<https://gqrx.dk/>。

### 切换界面语言

通过 **View → Language（视图 → 语言）** 选择 **System default（系统默认）**、**简体中文** 或 **English**。语言切换即时生效，无需重启；语言偏好会单独保存，不受接收设备配置文件切换影响。

默认选项为“系统默认”：简体中文系统使用中文界面，其他系统使用 English。程序会内嵌 Qt 简体中文翻译资源，使标准按钮和对话框也能显示中文。

中文界面保留 Hz、kHz、MHz、dB、dBFS、ppm、I/Q 等常用单位和缩写。无线电术语及翻译约定见[中文术语表](docs/translation-glossary.zh-CN.md)。

## 已知问题

请查看 GitHub 上的[问题跟踪器](https://github.com/gqrx-sdr/gqrx/issues)。

## 获取帮助与报告问题

Gqrx 的 Google 讨论组用于交流项目相关问题，包括安装帮助与故障排查：<https://groups.google.com/g/gqrx>。

提问时请详细说明遇到的问题、软硬件配置、已执行的操作步骤等。

也欢迎留在讨论组中帮助其他用户。如果所有用户支持工作都由开发者承担，就会挤占后续开发的时间。

## 从源代码构建

源代码托管在 GitHub：<https://github.com/gqrx-sdr/gqrx>。

编译 Gqrx 需要以下依赖：

- GNU Radio 3.8、3.9 或 3.10，包含以下组件：
  - gnuradio-runtime
  - gnuradio-analog
  - gnuradio-audio
  - gnuradio-blocks
  - gnuradio-digital
  - gnuradio-fft
  - gnuradio-filter
  - gnuradio-network（仅 GNU Radio 3.10）
  - gnuradio-pmt
- gr-iqbalance 库（可选）。
- 所需硬件的驱动程序：
  - UHD，通过 gr-uhd 使用；
  - FUNcube Dongle 和 FUNcube Dongle Pro+：[gr-funcube](https://github.com/dl1ksv/gr-funcube)；
  - RTL-SDR：<https://gitea.osmocom.org/sdr/rtl-sdr>；
  - HackRF：<https://github.com/mossmann/hackrf>；
  - Airspy：<https://github.com/airspy/airspyone_host>；
  - SoapySDR：<https://github.com/pothosware/SoapySDR>；
  - RFSpace 驱动已内置。
- gnuradio-osmosdr：<https://gitea.osmocom.org/sdr/gr-osmosdr>。
- pulseaudio 或 portaudio（仅 Linux，可选）。
- Qt 5 或 Qt 6，包含以下组件：
  - Core
  - GUI
  - Network
  - Widgets
  - Svg（运行时需要）
  - LinguistTools（构建和更新翻译目录需要，提供 `lrelease` 与 `lupdate`）
- Qt 简体中文翻译资源，必须包含 `qtbase_zh_CN.qm`。在 Debian/Ubuntu 上，对应软件包例如 `qttranslations5-l10n` 或 `qt6-translations-l10n`。
- CMake 3.5.0 或更新版本。

Gqrx 可通过 Qt Creator 或终端编译。

### 命令行构建

```sh
git clone https://github.com/gqrx-sdr/gqrx.git gqrx.git
cd gqrx.git
mkdir build
cd build
cmake ..
make
```

在某些系统上，CMake 默认的 Release 构建会因过度优化而性能不佳。如果遇到此情况，可在执行 CMake 之前尝试强制使用 `-O2`：

```sh
export CXXFLAGS=-O2
```

### 使用 Qt Creator 构建

```sh
git clone https://github.com/gqrx-sdr/gqrx.git gqrx.git
cd gqrx.git
mkdir build
```

1. 启动 Qt Creator，打开 `gqrx.git/CMakeLists.txt`。
2. 在构建位置对话框中选择 `gqrx.git/build`，然后继续。
3. 如果程序要求指定 CMake 可执行文件，请选择相应文件，然后继续。
4. 单击运行 CMake 的按钮，完成后单击完成。
5. 如需并行构建，可在 Projects 页面中的 Build Steps → Make → Additional arguments 中输入 `-j4`，并将 `4` 替换为处理器核心数。
6. 之后即可按通常方式使用 Qt Creator。

## 调试

可通过 `QT_LOGGING_RULES` 环境变量启用调试日志：

```sh
QT_LOGGING_RULES="*.debug=true;plotter.debug=false;qt.*.debug=false" gqrx
```

如需同时启用频谱绘图调试日志，请使用：

```sh
QT_LOGGING_RULES="*.debug=true;qt.*.debug=false" gqrx
```

## 致谢与许可证

Gqrx 由 Alexandru Csete OZ9AEC 设计和编写，采用 GNU 通用公共许可证（GPL）。

部分源代码改编自 Moe Weatley 的 Cutesdr，采用简化版 BSD 许可证。

以下个人和组织为 Gqrx 作出了贡献：

* Alex Grinkov
* Alexander Fasching
* Andrea Merello
* Andrea Montefusco, IW0HDV
* Andy Sloane
* Anthony Willard
* Anton Blanchard
* AsciiWolf
* Bastian Bloessl
* Ben Reese
* Bob McGwier, N4HY
* Brandonn Etheve
* charlylima
* Chris Kuethe
* Christian Lindner, DL2VCL
* Clayton Smith, VE3IRR
* Dallas Epperson
* Daniil Cherednik
* Darin Franklin
* Davide Gerhard
* Dominic Chen
* Doron Behar
* Doug Hammond
* Edouard Lafargue
* Elias Önal
* Federico Fuga
* Frank Brickle, AB2KT
* Frank Werner-Krippendorf, HB9FXQ
* Ganael Laplanche
* Gisle Vanem
* Göran Weinholt, SA6CJK
* Grigory Shipunov
* Gwenhael Goavec-Merou
* Herman Semenov
* James Yuzawa
* Jaroslav Škarvada
* Jeff Long
* Jiawei Chen
* Jiří Pinkava
* Joachim Schueth, DL2KCD
* Jon Bergli Heier
* Josh Blum
* Kate Adams
* Kenji Rikitake, JJ1BDX
* Kitware Inc.
* Konrad Beckmann
* Luna Gräfje
* luzpaz
* Marco Savelli
* Markus Kolb
* Michael Dickens
* Michael Lass
* Michael Tatarinov
* Moe Weatley
* Nadeem Hasan
* Nate Temple
* Nick Robinson, KE5YWP
* Nokia
* Oliver Grossmann, DH2WQ
* Pavel Milanes, CO7WT
* Pavel Stano
* Phil Vachon
* Radoslav Gerganov
* Rob Frohne
* Ron Economos, W6RZ
* Ruslan Migirov
* Russell Dwarshuis, KB8U
* Ryan Volz
* Shuyuan Liu
* Stefano Leucci
* Sultan Qasim Khan
* Sylvain Munaut
* Tarmo Tanilsoo
* Tomasz Lemiech
* Timothy Reaves
* Valentin Ochs
* Vesa Solonen
* Vincent Pelletier
* Vladisslav P
* Will Scales
* Wolfgang Fritz, DK7OB
* Youssef Touil
* Zero_Chaos

部分图标来自：

- GNOME 图标主题：由 GNOME 图标作者创作，采用 CC-SA 3.0 许可证；
- Tango 图标主题：由 Tango! 项目成员创作，属于公有领域；
- Mint-X 图标主题：由 Clement Lefebvre 创作，采用 GPL 许可证。

同时感谢 Volker Schroer 和 Alexey Bazhin，为 GNU Radio 和 Gqrx 引入 Funcube Dongle Pro+ 支持。

如果致谢名单中有所遗漏，请告知项目作者。

Alex OZ9AEC
