#!/bin/bash

CUR_DIR=$(pwd)
SDK_ROOT=$(realpath $CUR_DIR/../../..)

# 1. 自动从 .config 获取板子和芯片名，或者通过参数传入
# 获取板名称 (例如 demo68-nor)
BOARD_NAME=$1
if [ -z "$BOARD_NAME" ]; then
    if [ -f $SDK_ROOT/nuttx/.config ]; then
        BOARD_NAME=$(grep CONFIG_ARCH_BOARD_CUSTOM_NAME $SDK_ROOT/nuttx/.config | cut -d'=' -f2 | tr -d '"')
    fi
fi
# 获取芯片型号 (例如 d12x)
CHIP_NAME=$2
if [ -z "$CHIP_NAME" ]; then
    if [ -f $SDK_ROOT/nuttx/.config ]; then
        if grep -q "CONFIG_ARCH_CHIP_D12X=y" $SDK_ROOT/nuttx/.config; then
            CHIP_NAME="d12x"
        else
            CHIP_NAME=$(grep -E "CONFIG_ARCH_CHIP_[A-Z0-9]+" $SDK_ROOT/nuttx/.config | grep "=y" | head -n 1 | cut -d'_' -f4 | tr '[:upper:]' '[:lower:]' | cut -d'=' -f1)
        fi
    fi
fi

# 默认兜底
BOARD_NAME=${BOARD_NAME:-demo68-nor}
CHIP_NAME=${CHIP_NAME:-d12x}

echo ">>> Packing for Board: $BOARD_NAME, Chip: $CHIP_NAME"

BOARD_DIR=$SDK_ROOT/vendor/artinchip/boards/$CHIP_NAME/$BOARD_NAME
PRJ_OUT=$(realpath $CUR_DIR/prebuilt)
TOOLDIR=$SDK_ROOT/vendor/artinchip/tools/scripts
TOOLCHAIN_DIR=${SDK_ROOT}/vendor/artinchip/toolchain/bin/riscv64-unknown-elf-
OBJ_COPY=${TOOLCHAIN_DIR}objcopy
ELF_SIZE=${TOOLCHAIN_DIR}size

echo ">>> Starting $CHIP_NAME image packing..."

# 2. 从 boards 目录下同步最新的配置文件和资源
if [ -d $BOARD_DIR/pack ]; then
    echo ">>> Syncing board specific pack config from $BOARD_DIR/pack"
    cp -rv $BOARD_DIR/pack/* $PRJ_OUT/
fi

# 3. 准备基础文件 (ELF/Manifest) 并转为 nuttx.bin
if [ -f $SDK_ROOT/nuttx/nuttx.elf ]; then
    cp -f $SDK_ROOT/nuttx/nuttx.elf $PRJ_OUT/$CHIP_NAME.elf
    cp -f $SDK_ROOT/nuttx/nuttx.manifest $PRJ_OUT/nuttx.manifest

    # 直接将最新的 ELF 转为 nuttx.bin 以供 .its 使用
    echo ">>> Generating nuttx.bin from $CHIP_NAME.elf..."
    $OBJ_COPY -S -O binary -R .note -R .note.gnu.build-id -R .comment $PRJ_OUT/$CHIP_NAME.elf $PRJ_OUT/nuttx.bin

    # 在控制台显示大小信息
    $ELF_SIZE $PRJ_OUT/$CHIP_NAME.elf
fi

# 4. 文件系统内容准备 (SDK 资源安装)
python3 $TOOLDIR/fsinstall.py --sdkout $PRJ_OUT --clean rodata/,data/
python3 $TOOLDIR/fsinstall.py --sdkout $PRJ_OUT --src $SDK_ROOT/vendor/artinchip/pack/resources/rodata/ --dst rodata/
python3 $TOOLDIR/fsinstall.py --sdkout $PRJ_OUT --src $SDK_ROOT/vendor/artinchip/pack/resources/data/ --dst data/

# 6. 生成文件系统镜像 (FATFS 和 LittleFS)
pushd $PRJ_OUT > /dev/null
python3 $TOOLDIR/makefatfs.py --fullpart --volab default --cluster 8 --sector 512 --tooldir $TOOLDIR --inputdir rodata --outfile $PRJ_OUT/rodata.fatfs
python3 $TOOLDIR/makelittlefs.py --pagesize 256 --blocksize 4096 --tooldir $TOOLDIR --inputdir data/ --outfile $PRJ_OUT/data.lfs
popd > /dev/null

# 7. 生成私有资源 (PBP/Partition 组合)
python3 $TOOLDIR/mk_private_resource.py -v -l $PRJ_OUT/pbp_cfg.json,$PRJ_OUT/partition.json -o $PRJ_OUT/pbp_cfg.bin

# 8. 调用核心生成脚本封装成最终 .img 镜像
# 镜像文件名也动态化
IMG_NAME="${CHIP_NAME}_${BOARD_NAME}_v1.0.0.img"
python3 $TOOLDIR/mk_image.py -v -c $PRJ_OUT/image_cfg.json -d $PRJ_OUT

echo ">>> Image generated successfully at: $PRJ_OUT/$IMG_NAME"





