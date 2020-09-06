#include <std/stdint.h>
#include <std/printk.h>

extern "C" ssize_t sys_write(int fd, uint8_t*buf, size_t count) {
    int i = 0;
    while (i < count) {
        putchar(((int8_t*)buf)[i++]);
    }
    return 0;
}
