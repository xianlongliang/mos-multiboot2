#include <std/stdint.h>
#include <interrupt/keyboard.h>

extern "C" ssize_t sys_read(int fd, void *buf, size_t count) {
    int i = 0;
    while (i < count) {
        ((int8_t*)buf)[i++] = KeyboardIO::GetInstance()->Queue.Pop();
    }
    return 0;
}
