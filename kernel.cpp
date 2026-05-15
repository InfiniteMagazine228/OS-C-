extern "C" void kernel_main() {
    // Địa chỉ bộ nhớ đệm hiển thị văn bản VGA (Luôn cố định và chuẩn xác 100%)
    volatile char* video_memory = (volatile char*) 0xB8000;
    
    // Kích thước chuẩn của chế độ văn bản VGA: 80 cột x 25 dòng
    const int COLS = 80;
    const int ROWS = 25;

    // Lặp qua toàn bộ 2000 ô ký tự trên màn hình để vẽ giao diện nền
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            int offset = (y * COLS + x) * 2;
            
            // 1. Vẽ thanh Top Bar trên cùng (Dòng 0): Nền đỏ (0x40), chữ trắng (0x0F) -> Mã màu 0x4F
            if (y == 0) {
                video_memory[offset] = ' '; // Ô trống tạo màu nền
                video_memory[offset + 1] = 0x4F; 
            }
            // 2. Vẽ cửa sổ Setup ở giữa màn hình (Dòng 5 đến 19, Cột 15 đến 65): Nền xám (0x70)
            else if (y >= 5 && y <= 19 && x >= 15 && x <= 65) {
                // Vẽ thanh tiêu đề cửa sổ (Dòng 5): Nền xám sẫm (0x80), chữ trắng (0x0F) -> Mã màu 0x8F
                if (y == 5) {
                    video_memory[offset] = ' ';
                    video_memory[offset + 1] = 0x8F;
                } else {
                    video_memory[offset] = ' ';
                    video_memory[offset + 1] = 0x70; // Thân cửa sổ màu xám
                }
            }
            // 3. Toàn bộ vùng còn lại làm hình nền: Nền xanh lá/rêu (0x20)
            else {
                video_memory[offset] = ' ';
                video_memory[offset + 1] = 0x20;
            }
        }
    }

    /* ----------------------------------------------------------------------
       IN CHỮ LÊN GIAO DIỆN CÀI ĐẶT
       ---------------------------------------------------------------------- */
    // Hàm phụ in chữ tại tọa độ X, Y mong muốn kèm màu sắc
    auto print_string = [&](int start_x, int start_y, const char* str, unsigned char color_code) {
        for (int i = 0; str[i] != '\0'; i++) {
            int offset = (start_y * COLS + (start_x + i)) * 2;
            video_memory[offset] = str[i];
            video_memory[offset + 1] = color_code;
        }
    };

    // In tên hệ điều hành lên thanh Top Bar
    print_string(2, 0, "[DuyKhanhOS - Setup Wizard]", 0x4F);

    // In tiêu đề cửa sổ cài đặt
    print_string(17, 5, " macOS-RedStar Installer ", 0x8F);

    // In nội dung bên trong cửa sổ
    print_string(20, 8, "Welcome to DuyKhanhOS Setup Suite.", 0x70);
    print_string(20, 10, "This installer will configure your core system.", 0x70);
    print_string(20, 11, "Please prepare your virtual drive partition.", 0x70);

    // Vẽ nút bấm giả lập "[ Continue ]" màu xanh dương (0x1F) ở góc dưới cửa sổ
    print_string(48, 17, "[ Continue ]", 0x1F);

    // Dừng CPU an toàn để giữ màn hình
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
