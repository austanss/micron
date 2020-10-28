#!/usr/bin/env bash


mkdir -p ./iso/boot/grub
mkdir -p ./iso/System/Kernel/
cp "$1" ./iso/boot/grub/grub.cfg

# Copy the kernel...
cp "$2" ./iso/System/Kernel/microNET.kernel

# Generate kernel
"$3" -o ./microNET.iso ./iso
