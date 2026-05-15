extern "C" void kernel_main() {
    // Con trỏ tới bộ nhớ đệm video VGA (màn hình text)
    volatile char* video_memory = (volatile char*) 0xB8000;
    
    const char* str = "Hello from Custom C++ OS!";
    
    // Xóa màn hình và in chuỗi ký tự (Chữ trắng nền đen - 0x07)
    for(int i = 0; str[i] != '\0'; ++i) {
        video_memory[i * 2] = str[i];
        video_memory[i * 2 + 1] = 0x07;
    }
}
