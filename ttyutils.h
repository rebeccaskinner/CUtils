#ifndef __TTY_DEFS_H__
#define __TTY_DEFS_H__

#define CURSOR_ESC 0x1B
#define CURSOR_CMD "\x1B["

typedef struct termios termios_t;
typedef struct winsize winsize_t;

typedef struct
{
    int        device_fd;
    uint16_t   cursor_x;
    uint16_t   cursor_y;
    winsize_t  winsize;
    termios_t* unraw_buffer;
} tty_t;

#endif
