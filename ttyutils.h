#ifndef __TTY_UTILS_H__
#define __TTY_UTILS_H__
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

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

int writef(tty_t* tty, const char* fmt, ...);
tty_t* tty_new(int fd);
void tty_free(tty_t* tty);
void tty_update_termsize(tty_t* tty);
int tty_width(tty_t* tty);
int tty_height(tty_t* tty);
void tty_size(tty_t* tty, int* x, int* y);
int tty_get_cursor_x(tty_t* tty);
int tty_get_cursor_y(tty_t* tty);
void tty_get_cursor_position(tty_t* tty, int* x, int* y);
void tty_update_cursor_position(tty_t* tty);
void tty_set_cursor_position(tty_t* tty, int x, int y);
void tty_set_cursor_horiz(tty_t* tty, int x);
void tty_set_cursor_vert(tty_t* tty, int y);
void tty_move_cursor_position(tty_t* tty, int x, int y);
void tty_move_cursor_horiz(tty_t* tty, int x);
void tty_move_cursor_vert(tty_t* tty, int y);
void tty_scroll_down(tty_t* tty, unsigned int count);
int  tty_printf_right_align(tty_t* tty, const char* fmt, ...);
#endif
