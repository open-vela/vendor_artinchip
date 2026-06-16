
D12X-Demo68-nor 开发板对 openvela 的支持
简介
    本目录为 匠芯 D12X-Demo68-nor 开发板提供 openvela 支持，基于 trunk 分支。

开发板硬件细节、原理图和官方上手指南请参考匠芯官方文档：
    [D12X-Demo68-nor 开发板硬件细节](https://aicdoc.artinchip.com/topics/product/d12x-demo-v1.html)
    [D12X-Demo68-nor 开发板原理图](https://aicdoc.artinchip.com/topics/product/d12x-doc.html)

本板适配仅在 open-vela/nuttx 与 open-vela/vendor_artinchip 的 trunk 分支上可编译。

二、[D12X]demo68-nor开发板编译打包说明
1. 下载源码
2. D12X SDK位于/vendor/artinchip目录下
        SDK目录结构:
        vendor/artinchip/
        ├── apps/                 # 核心应用与 Demo (如 luncher_mini)
        ├── boards/               # 板级支持包 (包含启动代码、引脚配置)
        │   └── d12x/             # D12X 平台
        │       └── demo68-nor/ # 本开发板核心目录,系统启动脚本
        ├── chips/                # 芯片级驱动 (GPIO, SPI, TWI, etc.)
        │   └── artinchip/
                ├── common/             # 公共驱动层
                ├── d12x/               # D12X 平台驱动层
                ├── peripherals/         # 外设驱动层
        │       ├── drivers/      # 底层 HAL 与 RTOS 适配层
        │       └── drv/          # NuttX 驱动实现层
        ├── pack/               # 固件打包与产线工具 (Pack tools)
        ├── Make.defs             # 全局构建规则
        └── Kconfig               # 全局配置入口

支持的外设
ST7789    LCD（4.3" 480X272 RGB）	up_fbinitialize	      /dev/fb0
GT911     CTP,                      gt911_driver_init    /dev/input0
          SDCARD,                   aic_mmcsd_initialize  /dev/mmcsd1



3.通用编译说明
    首次编译前，需要先运行vendor/artinchip/tools/env.sh，下载并解压toolchain,或者参考简化编译打包说明
    D12x-DEMO68-V1-2开发板 通用编译命令可以采用以下命令
    ./build.sh vendor/artinchip/boards/d12x/demo68-nor/configs/nsh/ distclean -j8
    ./build.sh vendor/artinchip/boards/d12x/demo68-nor/configs/nsh/ -j8

4.通用打包说明
    cd vendor/artinchip/pack
    ./pack.sh

5.简化编译打包
    在首次编译前，可以将vendor/artinchip/tools/artbuild.sh用软连接到vendor/../
        ln -s ./vendor/artinchip/tools/artbuild.sh ./
     后续简化编译打包命令为以下命令：
        ./artbuild.sh 【proj】【opt】
        【proj】：为vendor/artinchip/boards/d12x/demo68-nor/configs下可以那看到的文件名
        【opt】：为常见的meunconfig distclean build pack为打包设置，详情可以使用 ./artbuild.sh help
        
        编译示例：
        在项目根目录下执行
                ./artbuild.sh nsh //编译+打包，
                ./artbuild.sh nsh build //单编译
                ./artbuild.sh nsh pack  //单打包
                ./artbuild.sh nsh menuconfig  //菜单配置

nuttx/ 目录下生成的产物：

文件	        大小	用途
nuttx	       约 11 MB	ELF，供 GDB 使用
nuttx.bin	   约 1.5 MB	平面镜像，供打包烧录

vendor/artinchip/pack/prebuilt
文件	                    大小	     用途
d12x_demo68-nor_v1.0.0.img  约5.3 MB	平面镜像，供打包烧录

烧录刷写方法：
    [D12X-Demo68-nor 开发板烧录刷写方法](https://aicdoc.artinchip.com/topics/quickstart/quick-start-chapter-tool-quickstart-d12x.html)


许可协议
本目录下所有文件均使用 Apache-2.0 协议（SPDX 标识符 Apache-2.0）；详见各文件头部声明。