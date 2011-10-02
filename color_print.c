#include <stdio.h>
#include <stdarg.h>
#include "color_print.h"

void color_start(int mode, int fg, int bg)
{
    printf("%c[%d;%d;%dm",CPF_ESCAPE,mode,30+fg,40+bg);
}

void color_stop()
{
    color_start(CPF_RESET,CPF_WHITE,CPF_BLACK);
}

int color_printf(int mode, int fg, int bg, const char* fmt, ...)
{
    va_list ap;
    int r;
    color_start(mode,fg,bg);
    va_start(ap,fmt);
    r = vprintf(fmt,ap);
    va_end(ap);
    color_stop();
    return r;
}
