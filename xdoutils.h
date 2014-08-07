#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <xdo.h>

int move_mouse_to(
    unsigned int xint,
    unsigned int yint,
    const char const * display_string
    );

void center_mouse();

int getRootWindowSize(int *w, int *h);

int getScreenSize(int *w, int *h);

void maximize_window(Display* dpy,Window w);
