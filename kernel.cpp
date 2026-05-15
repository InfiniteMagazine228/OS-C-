// Định nghĩa cấu trúc một nút bấm trong trình Setup
struct Button {
    int x, y, width, height;
    const char* text;
    unsigned int color;

    void draw() {
        // Gọi hàm vẽ hình chữ nhật lên màn hình
        draw_rect(x, y, width, height, color);
        // Gọi hàm vẽ chữ lên trên nút bấm
        draw_string(x + 10, y + 5, text, 0xFFFFFF); 
    }
};

extern "C" void kernel_main() {
    // 1. Khởi tạo driver đồ họa (Ví dụ: đặt màn hình về 1024x768, 32-bit màu)
    init_vesa_graphics();

    // 2. Vẽ thanh Menu trên cùng (Top Bar) kiểu macOS/RedStar (Màu xám/đỏ)
    draw_rect(0, 0, 1024, 30, 0xBB1111); // Màu đỏ RedStar
    draw_string(20, 8, "OS Installer", 0xFFFFFF);

    // 3. Vẽ cửa sổ Setup chính (Window)
    draw_rect(212, 134, 600, 500, 0xEEEEEE); // Nền xám nhạt macOS

    // 4. Tạo và vẽ nút "Tiếp tục"
    Button next_button = { 680, 580, 100, 35, "Continue", 0x007AFF }; // Màu xanh dương macOS
    next_button.draw();

    // Vòng lặp vô tận giữ hệ thống chạy và lắng nghe sự kiện chuột
    while(1) {
        // check_mouse_click();
    }
}
