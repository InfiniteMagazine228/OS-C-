# 1. Định nghĩa các hằng số cho Multiboot Header (Yêu cầu đồ họa)
.set ALIGN,    1<<0             # Căn lề các phân vùng nạp
.set MEMINFO,  1<<1             # Cung cấp bản đồ bộ nhớ
.set GRAPHICS, 1<<2             # CỜ QUAN TRỌNG: Yêu cầu GRUB bật chế độ đồ họa VESA
.set FLAGS,    ALIGN | MEMINFO | GRAPHICS
.set MAGIC,    0x1BADB002       # Số ma thuật nhận diện Multiboot
.set CHECKSUM, -(MAGIC + FLAGS) # Kiểm tra lỗi cấu trúc

# 2. Xây dựng cấu trúc Multiboot Header theo tiêu chuẩn đồ họa
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
# Các trường tùy chọn cấu hình đồ họa bổ sung cho GRUB
.long 0
.long 0
.long 0
.long 0
.long 0
.long 0                         # 0 = Khởi động ở chế độ đồ họa (Linear Framebuffer)
.long 1024                      # Chiều rộng màn hình mong muốn (Width)
.long 768                       # Chiều cao màn hình mong muốn (Height)
.long 32                        # Độ sâu màu (32-bit màu ARGB)

# 3. Cấp phát bộ nhớ cho Stack
.section .bss
.align 16
stack_bottom:
.skip 16384                     # Cấp 16KiB vùng nhớ đệm
stack_top:

# 4. Điểm khởi đầu của Hệ điều hành khi GRUB bàn giao quyền điều khiển
.section .text
.global _start
.type _start, @function
_start:
	mov $stack_top, %esp    # Thiết lập con trỏ Stack cho C++

	# TRUYỀN THAM SỐ SANG C++ CHUẨN: 
	# GRUB lưu thông tin Multiboot Info ở thanh ghi EBX và Magic ở EAX.
	# Ta phải đẩy chúng vào Stack theo đúng thứ tự để hàm kernel_main(mbi, magic) đọc được.
	push %eax               # Tham số thứ 2: magic
	push %ebx               # Tham số thứ 1: Con trỏ cấu trúc cấu hình mbi

	call kernel_main        # Gọi hàm chính trong file kernel.cpp

	cli
1:	hlt                     # Dừng CPU an toàn nếu kernel kết thúc hoặc thoát
	jmp 1b
