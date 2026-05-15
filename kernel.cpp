/* --------------------------------------------------------------------------
   CẤU TRÚC DỮ LIỆU MULTIBOOT ĐỂ ĐỌC THÔNG TIN ĐỒ HỌA TỪ GRUB
   -------------------------------------------------------------------------- */
struct multiboot_info {
    unsigned int flags;
    unsigned int mem_lower;
    unsigned int mem_upper;
    unsigned int boot_device;
    unsigned int cmdline;
    unsigned int mods_count;
    unsigned int mods_addr;
    unsigned int num;
    unsigned int size;
    unsigned int addr;
    unsigned int shndx;
    
    // Các trường thông tin về bộ đệm đồ họa (Linear Framebuffer)
    unsigned int vbe_control_info;
    unsigned int vbe_mode_info;
    unsigned short vbe_mode;
    unsigned short vbe_interface_seg;
    unsigned short vbe_interface_off;
    unsigned short vbe_interface_len;
    
    unsigned int framebuffer_addr_lower;
    unsigned int framebuffer_addr_upper;
    unsigned int framebuffer_pitch;
    unsigned int framebuffer_width;
    unsigned int framebuffer_height;
    unsigned char framebuffer_bpp;
    unsigned char framebuffer_type;
};

/* --------------------------------------------------------------------------
   CÁC BIẾN TOÀN CỤC LƯU TRỮ CẤU HÌNH MÀN HÌNH
   -------------------------------------------------------------------------- */
unsigned int* fb_address = 0;
unsigned int fb_width = 1024;
unsigned int fb_height = 768;
unsigned int fb_pitch = 4096;

/* --------------------------------------------------------------------------
   HÀM VẼ KHỐI ĐỒ HỌA SƠ KHẢI
   -------------------------------------------------------------------------- */
// Hàm vẽ một hình chữ nhật dựa trên hệ màu Hex (Vd: 0x007AFF)
void draw_rectangle(int start_x, int start_y, int width, int height, unsigned int hex_color) {
    if (fb_address == 0) return; // Không có bộ nhớ màn hình thì bỏ qua

    for (int y = start_y; y < start_y + height; y++) {
        // Kiểm tra giới hạn màn hình theo chiều dọc
        if (y >= (int)fb_height || y < 0) continue;

        for (int x = start_x; x < start_x + width; x++) {
            // Kiểm tra giới hạn màn hình theo chiều ngang
            if (x >= (int)fb_width || x < 0) continue;

            // Tính toán vị trí pixel chính xác trong bộ nhớ dựa trên thông số Pitch từ GRUB
            unsigned int* pixel_location = (unsigned int*)((unsigned char*)fb_address + (y * fb_pitch) + (x * 4));
            *pixel_location = hex_color;
        }
    }
}

/* --------------------------------------------------------------------------
   HÀM CHÍNH CỦA KERNEL (ĐIỂM VÀO HỆ ĐIỀU HÀNH)
   -------------------------------------------------------------------------- */
extern "C" void kernel_main(struct multiboot_info* mbi, unsigned int magic) {
    // Bước 1: Kiểm tra xem GRUB có truyền đúng cấu trúc Multiboot Info đồ họa không
    // Cờ số 11 (1 << 11 = 0x800) xác nhận thông tin Framebuffer có sẵn
    if (magic == 0x2BADB002 && (mbi->flags & (1 << 11))) {
        fb_address = (unsigned int*)(unsigned long)mbi->framebuffer_addr_lower;
        fb_width = mbi->framebuffer_width;
        fb_height = mbi->framebuffer_height;
        fb_pitch = mbi->framebuffer_pitch;
    } else {
        // Nếu không lấy được cấu hình tự động, ép gán địa chỉ mặc định của QEMU
        fb_address = (unsigned int*)0xFD000000;
        fb_width = 1024;
        fb_height = 768;
        fb_pitch = 1024 * 4;
    }

    /* ----------------------------------------------------------------------
       BẮT ĐẦU VẼ GIAO DIỆN SETUP (LAI MAC OS & REDSTAR OS)
       ---------------------------------------------------------------------- */
    
    // 1. Vẽ hình nền chính: Màu xanh rêu đậm quân đội đặc trưng của RedStar OS
    draw_rectangle(0, 0, fb_width, fb_height, 0x243A2B);

    // 2. Vẽ thanh Top Bar trên đỉnh màn hình: Màu đỏ cờ (RedStar OS Style)
    draw_rectangle(0, 0, fb_width, 32, 0xB51A1A);

    // 3. Vẽ một ô vuông nhỏ màu trắng bên góc trái thanh Top Bar giả lập nút Logo
    draw_rectangle(15, 8, 16, 16, 0xFFFFFF);

    // 4. Vẽ cửa sổ cài đặt chính (Setup Wizard Window) đặt ở giữa màn hình (macOS Style)
    // Tọa độ căn giữa cho màn hình kích thước 1024x768
    int win_x = (fb_width - 640) / 2;
    int win_y = (fb_height - 480) / 2;
    
    // Thân cửa sổ màu xám sáng nguyên bản của hệ điều hành Mac
    draw_rectangle(win_x, win_y, 640, 480, 0xEBEBEB); 
    
    // Thanh tiêu đề của cửa sổ (Title Bar) màu xám sẫm hơn một chút
    draw_rectangle(win_x, win_y, 640, 38, 0xD4D4D4);

    // Vẽ 3 nút đóng/thu nhỏ cửa sổ đặc trưng của Mac nằm ở góc trái Title Bar (Đỏ, Vàng, Xanh lá)
    draw_rectangle(win_x + 15, win_y + 13, 12, 12, 0xFF5F56); // Nút Đỏ
    draw_rectangle(win_x + 35, win_y + 13, 12, 12, 0xFFBD2E); // Nút Vàng
    draw_rectangle(win_x + 55, win_y + 13, 12, 12, 0x27C93F); // Nút Xanh lá

    // 5. Vẽ một vùng màu trắng lớn bên trong cửa sổ làm khu vực hiển thị nội dung Setup
    draw_rectangle(win_x + 40, win_y + 80, 560, 300, 0xFFFFFF);

    // 6. Vẽ nút bấm lệnh xác nhận "Tiếp tục" (Continue Button) màu xanh đậm macOS ở góc dưới bên phải
    draw_rectangle(win_x + 460, win_y + 410, 140, 40, 0x007AFF);

    // 7. Vẽ một thanh Dock ứng dụng ẩn mờ phía dưới đáy màn hình (macOS Layout)
    int dock_w = 400;
    int dock_h = 55;
    draw_rectangle((fb_width - dock_w) / 2, fb_height - dock_h - 10, dock_w, dock_h, 0x40FFFFFF); // Màu trắng trong suốt (Alpha giả lập)

    /* ----------------------------------------------------------------------
       DỪNG TIẾN TRÌNH CPU ĐỂ GIỮ NGUYÊN GIAO DIỆN
       ---------------------------------------------------------------------- */
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
