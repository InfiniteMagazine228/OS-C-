name: Build Custom C++ OS ISO

on:
  push:
    branches: [ "main" ]
  workflow_dispatch:

jobs:
  build-os:
    runs-on: ubuntu-24.04

    steps:
      - name: Checkout Code
        uses: actions/checkout@v4

      - name: Install Compiler and ISO Tools
        run: |
          sudo apt-get update
          sudo apt-get install -y gcc-i686-linux-gnu g++-i686-linux-gnu \
                                 grub-pc-bin xorriso mtools

      - name: Compile Source Files
        run: |
          # Biên dịch file Assembly
          i686-linux-gnu-as boot.s -o boot.o
          
          # Biên dịch file C++ (Tắt toàn bộ thư viện nền để chạy bare-metal)
          i686-linux-gnu-g++ -c kernel.cpp -o kernel.o \
            -ffreestanding -O2 -Wall -Wextra \
            -fno-exceptions -fno-rtti

      - name: Link Kernel Binary
        run: |
          i686-linux-gnu-gcc -T linker.ld -o myos.bin \
            -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

      - name: Structure ISO Directory
        run: |
          mkdir -p isodir/boot/grub
          cp myos.bin isodir/boot/myos.bin
          
          # Kiểm tra nếu chưa có grub.cfg thì tự động tạo file mặc định
          if [ ! -f grub.cfg ]; then
            cat <<EOF > isodir/boot/grub/grub.cfg
          menuentry "My Custom C++ OS" {
              multiboot /boot/myos.bin
              boot
          }
          EOF
          else
            cp grub.cfg isodir/boot/grub/grub.cfg
          fi

      - name: Build Bootable ISO
        run: |
          grub-mkrescue -o myos.iso isodir

      - name: Upload ISO Artifact
        uses: actions/upload-artifact@v4
        with:
          name: custom-os-iso
          path: myos.iso
