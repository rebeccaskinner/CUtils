#include <stdio.h>
#include <stdint.h>
#include "ttyutils.h"
#include "utils.h"

int main(int argc UNUSED, char** argv UNUSED)
{
    tty_t* tty = tty_new(0);
    for(int i = 0; i < 10 + tty_height(tty); i++) {
        tty_printf_right_align(tty,"%s - %d",argv[1],i);
        tty_scroll_down(tty,1);
        usleep(90000);
    }
    tty_free(tty);
}
