#!/bin/bash
set -e

# 获取脚本所在的真实物理路径（处理软核链接）
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
SCRIPT_DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

# 定位到 SDK 根目录 (由于该脚本在 vendor/artinchip/tools/，根目录是上三级)
SDK_ROOT=$(cd "$SCRIPT_DIR/../../../" && pwd)

# 自动安装 commit-msg 钩子 (方案3实现)
# 确保在 vendor/artinchip 项目中可以使用 Change-Id 进行代码评审
HOOKS_DIR="$SDK_ROOT/vendor/artinchip/.git/hooks"
REPO_HOOK="$SDK_ROOT/.repo/repo/hooks/commit-msg"

if [ -d "$HOOKS_DIR" ] && [ -f "$REPO_HOOK" ] && [ ! -f "$HOOKS_DIR/commit-msg" ]; then
    echo "Info: Installing commit-msg hook for vendor/artinchip..."
    cp "$REPO_HOOK" "$HOOKS_DIR/commit-msg"
    chmod +x "$HOOKS_DIR/commit-msg"
fi


# 帮助信息
function show_help() {
    echo "Usage: ./artbuild.sh [PROJECT] [OPTION]"
    echo ""
    echo "PROJECT:"
    echo "  nsh         Build the project for NSH"
    echo "  nsh_cxx     Build the project for NSH with C++ 14.1 support"
    echo "  xts_opus    Build the project for XTS OPUS"
    echo "Options:"
    echo "  (none)      Build the project and pack the image (default)"
    echo "  rebuild     Clean (distclean), build the project, and pack the image"
    echo "  pack        Only pack the image"
    echo "  help        Show this help message"
    echo "  [custom]    Pass custom options directly to build.sh (e.g., clean, menuconfig)"
    echo ""
    echo "Example:"
    echo "  ./artbuild.sh nsh          # Build and pack (default)"
    echo "  ./artbuild.sh nsh build    # Build "
    echo "  ./artbuild.sh nsh rebuild  # Full clean, build, and pack"
    echo "  ./artbuild.sh nsh distclean # Only run distclean via build.sh"
    echo "  ./artbuild.sh nsh clean     # Only run clean via build.sh"
}


if [ $# -lt 1 ] || [ "$1" == "help" ] || [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
    show_help
    exit 1
fi

pro=$1
if [ $# -ge 2 ]; then
opt=$2
fi

# 定义板级配置路径
BOARD_CONFIG_DIR="vendor/artinchip/boards/d12x/demo68-nor/configs/$pro"
BOARD_NAME=$(echo $BOARD_CONFIG_DIR | cut -d'/' -f5)
CHIP_NAME=$(echo $BOARD_CONFIG_DIR | cut -d'/' -f4)
IMAGE_NAME="${CHIP_NAME}_${BOARD_NAME}_v1.0.0.img"


# 检查是否请求帮助
if [ "$opt" == "help" ] || [ "$opt" == "-h" ] || [ "$opt" == "--help" ]; then
    show_help
    exit 0
fi

# 如果没有参数，默认进行编译和打包
if [ -z "$opt" ]; then
   echo "No option provided. Defaulting to: build + pack"
   cd "$SDK_ROOT"
   ./build.sh $BOARD_CONFIG_DIR/ -j8
   if [ $? -ne 0 ]; then
       echo "Build failed. Exiting."
       exit 1
   fi
   cd "$SDK_ROOT/vendor/artinchip/pack"
   ./pack.sh
   cp ./prebuilt/$IMAGE_NAME "$SDK_ROOT/"
   exit 0
fi

# 增加 build 参数：执行 build only
if [ "$opt" == "build" ]; then
   echo "No option provided. Defaulting to: build only"
   cd "$SDK_ROOT"
   ./build.sh $BOARD_CONFIG_DIR/ -j8
   exit 0
fi

# 增加 clean 参数：执行 distclean
if [ "$opt" == "clean" ]; then
   echo "Cleaning: distclean with path recovery"
   cd "$SDK_ROOT"
   
   # 1. 彻底移除引起冲突的各项
   rm -rf nuttx/arch/risc-v/src/chip
   rm -rf nuttx/include/arch
   rm -f nuttx/Make.defs

fi

# 增加 rebuild 参数：执行 distclean + build + pack
if [ "$opt" == "rebuild" ]; then
   echo "Rebuilding: distclean + build + pack"
   cd "$SDK_ROOT"
   
   # 1. 彻底移除引起冲突的各项
   rm -rf nuttx/arch/risc-v/src/chip
   rm -rf nuttx/include/arch
   rm -f nuttx/Make.defs

   # 2. 解析 defconfig 获取芯片绝对路径 (适配 SDK_ROOT)
   DEFCONFIG="$SDK_ROOT/$BOARD_CONFIG_DIR/defconfig"
   if [ -f "$DEFCONFIG" ]; then
       CHIP_VAL=$(grep "^CONFIG_ARCH_CHIP_CUSTOM_DIR=" "$DEFCONFIG" | cut -d'=' -f2 | tr -d '"')
       CHIP_REL=${CHIP_VAL#../} # 去除 ../
       CHIP_ABS="$SDK_ROOT/$CHIP_REL"
       
       if [ -d "$CHIP_ABS" ]; then
           echo "Runing pre-link for chip: $CHIP_ABS"
           mkdir -p nuttx/arch/risc-v/src
           ln -sf "$CHIP_ABS" nuttx/arch/risc-v/src/chip
       else
           echo "Warning: Chip dir from defconfig not found: $CHIP_ABS"
       fi
   fi

   # 3. 补齐板级配置链接
   MAKEDEFS_SRC="$SDK_ROOT/$BOARD_CONFIG_DIR/../../scripts/Make.defs"
   if [ -f "$MAKEDEFS_SRC" ]; then
       ln -sf "$MAKEDEFS_SRC" "$SDK_ROOT/nuttx/Make.defs"
   fi

   ./build.sh $BOARD_CONFIG_DIR/ distclean -j8
   ./build.sh $BOARD_CONFIG_DIR/ -j8
   if [ $? -ne 0 ]; then
       echo "Build failed. Exiting."
       exit 1
   fi
   cd "$SDK_ROOT/vendor/artinchip/pack"
   ./pack.sh
   cp ./prebuilt/$IMAGE_NAME "$SDK_ROOT/"
   exit 0
fi

echo "Building with option: $opt"

if [ "$opt"x != "packx" ]; then
   cd "$SDK_ROOT"
   ./build.sh $BOARD_CONFIG_DIR/ -j8 $opt
else
   cd "$SDK_ROOT/vendor/artinchip/pack"
   ./pack.sh
   cp ./prebuilt/$IMAGE_NAME "$SDK_ROOT/"
   echo "cp image completed,dir = $SDK_ROOT/"
fi