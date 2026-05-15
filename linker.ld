ENTRY(_start)

SECTIONS
{
    /* GRUB nạp kernel tại vị trí 1MB trong bộ nhớ */
    . = 1M;

    /* Đảm bảo phân vùng multiboot phải xuất hiện đầu tiên */
    .text BLOCK(4K) : ALIGN(4K)
    {
        KEEP(*(.multiboot))
        *(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
