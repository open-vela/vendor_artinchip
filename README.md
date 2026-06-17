# D12X-Demo68-nor 开发板对 openvela 的支持

[ [English](README_en.md) | 简体中文 ]

## 简介

本目录为 **匠芯 (ArtInChip) D12X-Demo68-nor** 开发板提供 openvela 支持。

开发板硬件细节、原理图和官方上手指南请参考匠芯官方文档：

- [D12X-Demo68-nor 开发板硬件细节](https://aicdoc.artinchip.com/topics/product/d12x-demo-v1.html)
- [D12X-Demo68-nor 开发板原理图](https://aicdoc.artinchip.com/topics/product/d12x-doc.html)

> ⚠️ **分支依赖**
>
> 本板适配仅在 `open-vela/nuttx` 与 `open-vela/vendor_artinchip` 的
> `dev-ai-contest-2026` 分支上可编译。从 `trunk` 或 `dev` 构建会失败，
> 因为芯片侧依赖尚未进入上游主线。

## 目录结构

```
vendor/artinchip/
├── apps/                       # 核心应用与 Demo (如 luncher_mini)
├── boards/                     # 板级支持包 (启动代码、引脚配置)
│   └── d12x/                   # D12X 平台
│       └── demo68-nor/         # 本开发板核心目录, 系统启动脚本
├── chips/                      # 芯片级驱动
│   └── artinchip/
│       ├── common/             # 公共驱动层
│       ├── d12x/               # D12X 平台驱动层
│       ├── peripherals/        # 外设驱动层
│       ├── drivers/            # 底层 HAL 与 RTOS 适配层
│       └── drv/                # NuttX 驱动实现层
├── pack/                       # 固件打包与产线工具 (Pack tools)
├── tools/                      # 工具链下载与构建脚本 (env.sh / artbuild.sh)
├── Make.defs                   # 全局构建规则
└── Kconfig                     # 全局配置入口
```

## 支持的外设

| 外设 | 驱动 | 设备节点 |
|------|------|----------|
| ST7789 LCD (4.3" 480×272 RGB) | `up_fbinitialize` | `/dev/fb0` |
| GT911 电容触摸屏 (CTP) | `gt911_driver_init` | `/dev/input0` |
| SD 卡 (SDCARD) | `aic_mmcsd_initialize` | `/dev/mmcsd1` |

## 可用配置

`boards/d12x/demo68-nor/configs/` 下提供以下配置：

| 配置 | 说明 |
|------|------|
| `nsh` | 基础 NSH 命令行 |
| `nsh_cxx` | NSH + C++ 支持 |
| `nsh_lvgl` | NSH + LVGL 图形 |
| `nsh_xts` | NSH + XTS 测试套件 |
| `xts_opus` | XTS + Opus 音频 |

## 编译

### 准备工具链

首次编译前，先运行 `env.sh` 下载并解压工具链：

```bash
./vendor/artinchip/tools/env.sh
```

### 通用编译

```bash
# 清理 + 编译
./build.sh vendor/artinchip/boards/d12x/demo68-nor/configs/nsh/ distclean -j8
./build.sh vendor/artinchip/boards/d12x/demo68-nor/configs/nsh/ -j8
```

### 打包

```bash
cd vendor/artinchip/pack
./pack.sh
```

## 简化编译打包

首次编译前，可将 `artbuild.sh` 软链接到项目根目录：

```bash
ln -s ./vendor/artinchip/tools/artbuild.sh ./
```

之后用统一入口编译打包：

```bash
./artbuild.sh <proj> <opt>
```

- `<proj>`：`vendor/artinchip/boards/d12x/demo68-nor/configs/` 下的配置名（如 `nsh`）
- `<opt>`：操作类型，常见 `menuconfig` / `distclean` / `build` / `pack`；详见 `./artbuild.sh help`

示例（在项目根目录执行）：

```bash
./artbuild.sh nsh              # 编译 + 打包
./artbuild.sh nsh build        # 仅编译
./artbuild.sh nsh pack         # 仅打包
./artbuild.sh nsh menuconfig   # 菜单配置
```

## 构建产物

`nuttx/` 目录下生成：

| 文件 | 大小 | 用途 |
|------|-----:|------|
| `nuttx` | 约 11 MB | ELF，供 GDB 使用 |
| `nuttx.bin` | 约 1.5 MB | 平面镜像，供打包烧录 |

`vendor/artinchip/pack/prebuilt/` 下生成：

| 文件 | 大小 | 用途 |
|------|-----:|------|
| `d12x_demo68-nor_v1.0.0.img` | 约 5.3 MB | 打包后的完整烧录镜像 |

## 烧录

详细烧录刷写方法请参考匠芯官方文档：

- [D12X-Demo68-nor 开发板烧录刷写方法](https://aicdoc.artinchip.com/topics/quickstart/quick-start-chapter-tool-quickstart-d12x.html)

## 许可协议

本目录下所有文件均使用 Apache-2.0 协议（SPDX 标识符 `Apache-2.0`）；详见各文件头部声明。
