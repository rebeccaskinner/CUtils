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

static void cursor_init() __attribute__((constructor));

int writef(int fd, const char* fmt, ...)
{
    char* s;
    int   l;
    va_list ap;
    va_start(ap,fmt);
    l = vasprintf(&s,fmt,ap);
    va_end(ap);
    return write(fd,s,l);
}

int raw_tty(int fd)
{
    struct termios tty;
    if(! isatty(fd)) return -1;
    if(-1 == tcgetattr(fd,&saved_tty))
    {
        fprintf(stderr,"Error getting terminal info: %s\n",strerror(errno));
        return -1;
    }
    term_saved = 1;

    tty = saved_tty;
    tty.c_iflag &= ~(BRKINT | ICRNL  | ISTRIP | IXON);
    tty.c_cflag &= ~(CSIZE  | PARENB);
    tty.c_cflag |= CS8;
    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

    if(-1 == tcsetattr(fd,TCSAFLUSH,&tty))
    {
        fprintf(stderr,"Error setting terminal info: %s\n",strerror(errno));
        return -1;
    }

    return 0;
}

int unraw_tty(int fd)
{
    if(!isatty(fd)) return -1;
    if(!term_saved) return 0;
    return tcsetattr(fd,TCSAFLUSH,&saved_tty);
}

static void cursor_init()
{
    cursor_pos_x = 0;
    cursor_pos_y = 0;
}

int get_term_size(int fd, uint32_t* x, uint32_t* y)
{
    struct winsize w;
    if(!isatty(fd)) return 1;
    ioctl(fd,TIOCGWINSZ,&w);
    *x = w.ws_row;
    *y = w.ws_col;
    return 0;
}

int get_term_width(int fd)
{
    uint32_t x, y;
    get_term_size(fd,&x,&y);
    return y;
}

int get_term_height(int fd)
{
    uint32_t x, y;
    get_term_size(fd,&x,&y);
    return x;
}

void update_cursor(int fd)
{
    writef(fd,"%c[%d;%dH",CURSOR_ESC,cursor_pos_x,cursor_pos_y);
}

void get_cursor_position(int* x, int* y)
{
    assert(x && y);
    *x = cursor_pos_x;
    *y = cursor_pos_y;
}

void move_cursor(int fd, int x, int y)
{
    int max_x, max_y;
    get_term_size(fd, &max_x, &max_y);
    cursor_pos_x = CONSTRAIN(0,max_x,(cursor_pos_x + x));
    cursor_pos_y = CONSTRAIN(0,max_y,(cursor_pos_y + y));
    update_cursor(fd);
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
