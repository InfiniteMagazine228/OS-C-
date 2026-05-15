/* --------------------------------------------------------------------------
   CÁC TIỆN ÍCH GIAO TIẾP PHẦN CỨNG ĐỂ ĐỌC CỔNG I/O (INB/OUTB)
   -------------------------------------------------------------------------- */
inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

/* --------------------------------------------------------------------------
   CẤU HÌNH MÀN HÌNH VÀ HÀM VẼ GIAO DIỆN TEXT MODE
   -------------------------------------------------------------------------- */
volatile char* video_memory = (volatile char*) 0xB8000;
const int COLS = 80;
const int ROWS = 25;

void draw_layout(const char* title, const char* win_title) {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            int offset = (y * COLS + x) * 2;
            if (y == 0) {
                video_memory[offset] = ' ';
                video_memory[offset + 1] = 0x4F; // Nền đỏ, chữ trắng (Top Bar)
            } else if (y >= 5 && y <= 19 && x >= 15 && x <= 65) {
                if (y == 5) {
                    video_memory[offset] = ' ';
                    video_memory[offset + 1] = 0x8F; // Tiêu đề cửa sổ (Xám sẫm)
                } else {
                    video_memory[offset] = ' ';
                    video_memory[offset + 1] = 0x70; // Thân cửa sổ (Xám sáng)
                }
            } else {
                video_memory[offset] = ' ';
                video_memory[offset + 1] = 0x20; // Hình nền (Xanh rêu)
            }
        }
    }

    auto print_inline = [](int start_x, int start_y, const char* s, unsigned char c) {
        for (int i = 0; s[i] != '\0'; i++) {
            int off = (start_y * COLS + (start_x + i)) * 2;
            video_memory[off] = s[i];
            video_memory[off + 1] = c;
        }
    };

    print_inline(2, 0, title, 0x4F);
    print_inline(17, 5, win_title, 0x8F);
}

void print_string(int start_x, int start_y, const char* str, unsigned char color_code) {
    for (int i = 0; str[i] != '\0'; i++) {
        int offset = (start_y * COLS + (start_x + i)) * 2;
        video_memory[offset] = str[i];
        video_memory[offset + 1] = color_code;
    }
}

/* --------------------------------------------------------------------------
   HÀM DELAY SƠ KHẢI DỰA TRÊN CHU KỲ CPU (MÔ PHỎNG THỜI GIAN CHẠY)
   -------------------------------------------------------------------------- */
void sleep(int iterations) {
    for (volatile int i = 0; i < iterations * 50000; i++) {
        __asm__ __volatile__("nop"); // Lệnh không làm gì để kéo dài thời gian
    }
}

/* --------------------------------------------------------------------------
   DRIVER BÀN PHÍM SƠ CẤP (KEYBOARD DRIVER)
   -------------------------------------------------------------------------- */
unsigned char get_scancode() {
    // Đợi cho đến khi bit số 0 của cổng trạng thái (0x64) bằng 1 (Bàn phím có dữ liệu mới)
    while ((inb(0x64) & 1) == 0);
    return inb(0x60); // Đọc trực tiếp mã phím từ cổng dữ liệu 0x60
}

/* --------------------------------------------------------------------------
   HÀM VẼ VÀ CHẠY THANH TIẾN TRÌNH CÀI ĐẶT TỰ ĐỘNG
   -------------------------------------------------------------------------- */
void run_installation_progress() {
    draw_layout("[DuyKhanhOS - Installing]", " System Installation ");
    print_string(20, 8, "Extracting macOS-RedStar kernel core...", 0x70);
    print_string(20, 10, "Setting up basic CLI and shell modules...", 0x70);
    
    // Khởi tạo thanh Progress Bar trống ban đầu
    print_string(18, 13, "[                               ] 0%", 0x72);

    // Mảng ký tự hiển thị phần trăm thủ công vì bare-metal không có hàm sprintf()
    const char* pct_strings[] = {
        " 0%", "10%", "20%", "30%", "40%", "55%", "70%", "85%", "95%", "100%"
    };

    // Vòng lặp tăng dần thanh trạng thái đồ họa văn bản
    for (int step = 0; step < 10; step++) {
        sleep(80); // Đợi một lúc trước khi tăng thanh tiến trình tiếp theo
        
        // Vẽ thêm các ký tự '=' đại diện cho thanh dung lượng đang đầy dần
        for (int fill = 0; fill <= step * 3; fill++) {
            print_string(19 + fill, 13, "=", 0x1A); // Màu nền xanh lam nổi bật
        }
        // Ghi đè con số phần trăm tương ứng vào cuối thanh tiến trình
        print_string(51, 13, pct_strings[step], 0x72);
    }

    // Khi đạt 100%, thay đổi trạng thái giao diện hoàn tất cài đặt
    print_string(20, 15, "SUCCESS: Core files copy completely!", 0x2F); // Nền xanh lá chữ trắng
    print_string(45, 17, "[ Finish Installation ]", 0x1F); // Sáng đèn nút bấm Kết thúc
    print_string(20, 19, "-> Press ENTER again to reboot system...", 0x74);
}

/* --------------------------------------------------------------------------
   HÀM CHÍNH KERNEL
   -------------------------------------------------------------------------- */
int current_stage = 1;

extern "C" void kernel_main() {
    // Vẽ màn hình Chào mừng (Trang 1) ban đầu
    draw_layout("[DuyKhanhOS - Setup Wizard]", " macOS-RedStar Installer ");
    print_string(20, 8, "Welcome to DuyKhanhOS Setup Suite.", 0x70);
    print_string(20, 10, "This installer will configure your core system.", 0x70);
    print_string(20, 11, "Please prepare your virtual drive partition.", 0x70);
    print_string(48, 17, "[ Continue ]", 0x1F); // Làm nổi bật nút bằng màu xanh dương
    print_string(20, 15, "-> Click window then press ENTER to continue...", 0x74);

    // Vòng lặp liên tục kiểm tra tín hiệu phần cứng (Event Loop)
    while (1) {
        unsigned char scancode = get_scancode();

        // Kiểm tra xem người dùng có bấm phím ENTER hay không (Mã quét 0x1C)
        if (scancode == 0x1C) {
            if (current_stage == 1) {
                current_stage = 2; // Chuyển dịch trạng thái logic hệ thống
                run_installation_progress(); // Kích hoạt hiệu ứng chạy thanh tiến trình
            } 
            else if (current_stage == 2) {
                // Nếu ấn ENTER lần nữa sau khi cài xong, xóa màn hình giả lập khởi động lại
                for (int i = 0; i < COLS * ROWS * 2; i += 2) {
                    video_memory[i] = ' ';
                    video_memory[i + 1] = 0x07;
                }
                print_string(30, 12, "Rebooting DuyKhanhOS...", 0x0F);
                while(1) { __asm__ __volatile__("hlt"); }
            }
        }
    }
}
