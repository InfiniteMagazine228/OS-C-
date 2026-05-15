extern "C" void kernel_main() {
    // Con trỏ tới bộ nhớ đệm video VGA (màn hình chế độ văn bản)
    volatile char* video_memory = (volatile char*) 0xB8000;
    
    // Chuỗi ký tự hiển thị chào mừng OS của bạn
    const char* str = "Hello from Custom C++ OS (MacOS Style)!";
    
    // Xóa màn hình và in chuỗi ký tự (Chữ trắng nền đen - đặc tính màu 0x07)
    for(int i = 0; str[i] != '\0'; ++i) {
        video_memory[i * 2] = str[i];
        video_memory[i * 2 + 1] = 0x07;
    }

    // Vòng lặp vô tận để giữ CPU không bị tắt
    while(1) {
        __asm__ __volatile__("hlt");
    }
}
