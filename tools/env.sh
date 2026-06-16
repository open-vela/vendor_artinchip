#!/bin/bash

TOOLCHAIN_URL="https://occ-oss-prod.oss-cn-hangzhou.aliyuncs.com/resource//1751370399722/Xuantie-900-gcc-elf-newlib-x86_64-V3.2.0-20250627.tar.gz"
TOOLCHAIN_DIR=$(dirname "$(dirname "$0")")/toolchain
TOOLCHAIN_TAR=$(basename "$TOOLCHAIN_URL")

mkdir -p "$TOOLCHAIN_DIR"

cd "$TOOLCHAIN_DIR"

if [ ! -f "$TOOLCHAIN_TAR" ]; then
    echo "Downloading toolchain: $TOOLCHAIN_URL"
    if ! curl -L -o "$TOOLCHAIN_TAR" "$TOOLCHAIN_URL"; then
        echo "Error: Failed to download toolchain"
        exit 1
    fi
fi

if [ ! -d "bin" ]; then
    echo "Extracting toolchain..."
    if ! tar -xzf "$TOOLCHAIN_TAR" --strip-components=1; then
        echo "Error: Failed to extract toolchain"
        exit 1
    fi
fi

echo "Toolchain setup completed."