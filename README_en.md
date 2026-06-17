# openvela Support for the D12X-Demo68-nor Board

[ English | [简体中文](README.md) ]

## Introduction

This directory provides openvela support for the **ArtInChip D12X-Demo68-nor**
development board.

For board hardware details, schematics and the official getting-started
guide, see the upstream ArtInChip documentation:

- [D12X-Demo68-nor hardware overview](https://aicdoc.artinchip.com/topics/product/d12x-demo-v1.html)
- [D12X-Demo68-nor schematics](https://aicdoc.artinchip.com/topics/product/d12x-doc.html)

> ⚠️ **Branch dependency**
>
> This board overlay only builds on the `dev-ai-contest-2026` branch of
> `open-vela/nuttx` and `open-vela/vendor_artinchip`. Building it from
> `trunk` or `dev` will fail because the chip-side dependencies are not
> yet upstream.

## Directory Structure

```
vendor/artinchip/
├── apps/                       # core apps and demos (e.g. luncher_mini)
├── boards/                     # board support packages (boot code, pinmux)
│   └── d12x/                   # D12X platform
│       └── demo68-nor/         # this board: core dir, init scripts
├── chips/                      # chip-level drivers
│   └── artinchip/
│       ├── common/             # common driver layer
│       ├── d12x/               # D12X platform driver layer
│       ├── peripherals/        # peripheral driver layer
│       ├── drivers/            # low-level HAL and RTOS adaptation
│       └── drv/                # NuttX driver implementation
├── pack/                       # firmware packaging / production tools
├── tools/                      # toolchain download & build scripts (env.sh / artbuild.sh)
├── Make.defs                   # global build rules
└── Kconfig                     # global configuration entry
```

## Supported Peripherals

| Peripheral | Driver | Device node |
|------------|--------|-------------|
| ST7789 LCD (4.3" 480×272 RGB) | `up_fbinitialize` | `/dev/fb0` |
| GT911 capacitive touch (CTP) | `gt911_driver_init` | `/dev/input0` |
| SD card (SDCARD) | `aic_mmcsd_initialize` | `/dev/mmcsd1` |

## Available Configurations

The following configs live under `boards/d12x/demo68-nor/configs/`:

| Config | Description |
|--------|-------------|
| `nsh` | Basic NSH shell |
| `nsh_cxx` | NSH + C++ support |
| `nsh_lvgl` | NSH + LVGL graphics |
| `nsh_xts` | NSH + XTS test suite |
| `xts_opus` | XTS + Opus audio |

## Build

### Prepare the toolchain

Before the first build, run `env.sh` to download and extract the toolchain:

```bash
./vendor/artinchip/tools/env.sh
```

### Standard build

```bash
# clean + build
./build.sh vendor/artinchip/boards/d12x/demo68-nor/configs/nsh/ distclean -j8
./build.sh vendor/artinchip/boards/d12x/demo68-nor/configs/nsh/ -j8
```

### Packaging

```bash
cd vendor/artinchip/pack
./pack.sh
```

## Simplified build & packaging

Before the first build, you can symlink `artbuild.sh` to the project root:

```bash
ln -s ./vendor/artinchip/tools/artbuild.sh ./
```

Then use the unified entry point for build and packaging:

```bash
./artbuild.sh <proj> <opt>
```

- `<proj>`: a config name under `vendor/artinchip/boards/d12x/demo68-nor/configs/` (e.g. `nsh`)
- `<opt>`: the operation — commonly `menuconfig` / `distclean` / `build` / `pack`; see `./artbuild.sh help`

Examples (run from the project root):

```bash
./artbuild.sh nsh              # build + pack
./artbuild.sh nsh build        # build only
./artbuild.sh nsh pack         # pack only
./artbuild.sh nsh menuconfig   # menuconfig
```

## Build Artefacts

Produced under `nuttx/`:

| File | Size | Purpose |
|------|-----:|---------|
| `nuttx` | ~11 MB | ELF, used by GDB |
| `nuttx.bin` | ~1.5 MB | flat image for packaging/flashing |

Produced under `vendor/artinchip/pack/prebuilt/`:

| File | Size | Purpose |
|------|-----:|---------|
| `d12x_demo68-nor_v1.0.0.img` | ~5.3 MB | full packaged flashing image |

## Flash

For the detailed flashing procedure, see the official ArtInChip documentation:

- [D12X-Demo68-nor flashing guide](https://aicdoc.artinchip.com/topics/quickstart/quick-start-chapter-tool-quickstart-d12x.html)

## License

All files in this directory are licensed under Apache-2.0 (SPDX identifier
`Apache-2.0`); see individual file headers for details.
