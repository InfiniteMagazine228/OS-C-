# Định nghĩa các hằng số cho Multiboot header
.set ALIGN,    1<<0             # Căn lề các phân vùng nạp
.set MEMINFO,  1<<1             # Cung cấp bản đồ bộ nhớ
.set FLAGS,    ALIGN | MEMINFO  # Cờ Multiboot
.set MAGIC,    0x1BADB002       # Số ma thuật giúp bootloader nhận diện
.set CHECKSUM, -(MAGIC + FLAGS) # Kiểm tra lỗi

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384 # Cấp phát 16KiB cho stack
stack_top:

.section .text
.global _start
.type _start, @function
_start:
	mov $stack_top, %esp    # Thiết lập con trỏ stack
	call kernel_main        # Gọi hàm chính bằng C++
	cli
1:	hlt                     # Dừng CPU nếu kernel thoát
	jmp 1b
