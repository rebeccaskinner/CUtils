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

static unsigned short cursor_pos_x;
static unsigned short cursor_pos_y;
static int term_saved = 0;
static struct termios saved_tty;

#define CURSOR_ESC 0x1B
#define CURSOR_CMD "\27["
#define CONSTRAIN(min,max,val) \
({ \
 typeof(val) _v = (val); \
 (_v >= min) ? ( (_v <= max) ? _v : max) : min; \
})

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

int writef(tty_t* tty, const char* fmt, ...)
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

static int raw_tty(tty_t* tty)
{
    struct termios raw_tty;
    assert(tty);
    if(!tty->unraw_tty)
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
    writef(tty->device_fd,"%c[%d;%dH",CURSOR_ESC,tty->cursor_x,tty->cursor_y);
}

void tty_set_cursor_position(tty_t* tty, int x, int y)
{
    assert(tty);
    tty->cursor_x = CONSTRAINT(0,tty_width(tty),x);
    tty->cursor_y = CONSTRAINT(0,tty_height(tty),y);
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
    for(int i = 0; i < count; i++)
    {

    }
}

void advance_line(int fd)
{
    int max_x = get_term_height(fd);
    if(cursor_pos_x >= max_x)
        writef(fd,"%cD",CURSOR_ESC);
    cursor_pos_x = CONSTRAIN(0,max_x,(cursor_pos_x + 1));
    cursor_pos_y = 0;
    update_cursor(fd);
}

int printf_right_align(int fd, const char* fmt, ...)
{
    char* s;
    int   l;
    int   w = get_term_width(0);
    va_list ap;
    va_start(ap,fmt);
    l = vasprintf(&s,fmt,ap);
    va_end(ap);
    if(0 >= w)
        return printf("%s",s);
    move_cursor(fd,0,w-l);
    writef(fd,"%s",s);
}

#ifdef TEST
int main(int argc, char** argv)
{
    raw_tty(0);
    for(int i = 0; i < 10 + (get_term_height(0)); i++) {
        printf_right_align(0,"%s - %d",argv[1],i);
        advance_line(0);
        usleep(50000);
    }
    unraw_tty(0);
}
#endif
