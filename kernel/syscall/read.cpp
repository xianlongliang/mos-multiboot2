#include <std/stdint.h>
#include <interrupt/keyboard.h>

extern "C" ssize_t sys_read(int fd, uint8_t *buf, size_t count)
{
    size_t i = 0;
    while (i < count)
    {
        ((int8_t *)buf)[i++] = KeyboardIO::GetInstance()->Queue.Pop();
    }
    return 0;
}
