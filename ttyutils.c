#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/ioctl.h>

#include "utils.h"
#include "ttyutils.h"

#define CONSTRAIN(min,max,val) \
({ \
 typeof(val) _v = (val); \
 (_v >= min) ? ( (_v <= max) ? _v : max) : min; \
})

static int raw_tty(tty_t* tty);
static int unraw_tty(tty_t* tty);

static int raw_tty(tty_t* tty)
{
    struct termios raw_tty;
    assert(tty);
    if(!tty->unraw_buffer)
        tty->unraw_buffer = malloc(sizeof(termios_t));

    if(-1 == tcgetattr(tty->device_fd, tty->unraw_buffer))
    {
        fprintf(stderr,"Error getting terminal info: %s\n",strerror(errno));
        free(tty->unraw_buffer);
        tty->unraw_buffer = NULL;
        return -1;
    }

    memcpy(&raw_tty,tty->unraw_buffer,sizeof(tty_t));
    raw_tty.c_iflag &= ~(BRKINT | ICRNL  | ISTRIP | IXON);
    raw_tty.c_cflag &= ~(CSIZE  | PARENB);
    raw_tty.c_cflag |= CS8;
    raw_tty.c_oflag &= ~OPOST;
    raw_tty.c_cc[VMIN] = 1;
    raw_tty.c_cc[VTIME] = 0;

    if(-1 == tcsetattr(tty->device_fd,TCSAFLUSH,&raw_tty))
    {
        fprintf(stderr,"Error setting terminal info: %s\n",strerror(errno));
        return -1;
    }

    return 0;
}

static int unraw_tty(tty_t* tty)
{
    assert(tty);
    if(!tty->unraw_buffer) return 0;
    return tcsetattr(tty->device_fd,TCSAFLUSH,tty->unraw_buffer);
}

int tty_writef(tty_t* tty, const char* fmt, ...)
{
    char* s;
    int   l;
    int   r;
    va_list ap;
    assert(tty && fmt);
    va_start(ap,fmt);
    l = vasprintf(&s,fmt,ap);
    va_end(ap);
    r = write(tty->device_fd,s,l);
    free(s);
    return r;
}

tty_t* tty_new(int fd)
{
    tty_t* tty;
    if(!isatty(fd)) return NULL;
    tty = malloc0(sizeof(tty_t));
    tty->device_fd = fd;
    tty_update_termsize(tty);
    raw_tty(tty);
    return tty;
}

void tty_free(tty_t* tty)
{
    assert(tty);
    unraw_tty(tty);
    if(LIKELY(NULL != tty->unraw_buffer))
        free(tty->unraw_buffer);
    free(tty);
}
void tty_update_termsize(tty_t* tty)
{
    assert(tty);
    ioctl(tty->device_fd,TIOCGWINSZ,&tty->winsize);
}

int tty_width(tty_t* tty)
{
    assert(tty);
    return tty->winsize.ws_col;
}

int tty_height(tty_t* tty)
{
    assert(tty);
    return tty->winsize.ws_row;
}

void tty_size(tty_t* tty, int* x, int* y)
{
    assert(tty && x && y);
    *x = tty_width(tty);
    *y = tty_height(tty);
}

int tty_get_cursor_x(tty_t* tty)
{
    assert(tty);
    return tty->cursor_x;
}

int tty_get_cursor_y(tty_t* tty)
{
    assert(tty);
    return tty->cursor_y;
}

void tty_get_cursor_position(tty_t* tty, int* x, int* y)
{
    assert(tty && x && y);
    *x = tty->cursor_x;
    *y = tty->cursor_y;
}

void tty_update_cursor_position(tty_t* tty)
{
    assert(tty);
    tty_writef(tty,"%c[%d;%dH",CURSOR_ESC,tty->cursor_y,tty->cursor_x);
}

void tty_set_cursor_position(tty_t* tty, int x, int y)
{
    assert(tty);
    tty->cursor_x = CONSTRAIN(0,tty_width(tty),x);
    tty->cursor_y = CONSTRAIN(0,tty_height(tty),y);
    tty_update_cursor_position(tty);
}

void tty_set_cursor_horiz(tty_t* tty, int x)
{
    assert(tty);
    tty_set_cursor_position(tty,x,tty->cursor_y);
}

void tty_set_cursor_vert(tty_t* tty, int y)
{
    assert(tty);
    tty_set_cursor_position(tty,tty->cursor_x,y);
}

void tty_move_cursor_position(tty_t* tty, int x, int y)
{
    assert(tty);
    tty->cursor_x = CONSTRAIN(0,tty_width(tty),tty->cursor_x + x);
    tty->cursor_y = CONSTRAIN(0,tty_height(tty),tty->cursor_y + y);
    tty_update_cursor_position(tty);
}

void tty_move_cursor_horiz(tty_t* tty, int x)
{
    tty_move_cursor_position(tty,x,0);
}

void tty_move_cursor_vert(tty_t* tty, int y)
{
    tty_move_cursor_position(tty,0,y);
}

void tty_scroll_down(tty_t* tty, unsigned int count)
{
    assert(tty);
    for(unsigned int i = 0; i < count; i++)
    {
        if(tty->cursor_x >= tty_height(tty))
        {
            tty_writef(tty,"%cD",CURSOR_ESC);
        }
        tty_move_cursor_vert(tty,1);
        tty_set_cursor_horiz(tty,1);
    }
}

int tty_printf_right_align(tty_t* tty, const char* fmt, ...)
{
    char*    s;
    int      l;
    int      w;
    va_list ap;

    assert(tty && fmt);

    w = tty_width(tty);
    va_start(ap,fmt);
    l = vasprintf(&s,fmt,ap);
    va_end(ap);
    if(0 >= w) return printf("%s",s);
    tty_set_cursor_horiz(tty,w-l);
    tty_writef(tty,"%s",s);
    return l;
}
