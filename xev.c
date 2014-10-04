/*

Copyright (c) 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

/*
 * Author:  Jim Fulton, MIT X Consortium
 * Hacked: IteratedLateralus ( http://github.com/iteratedlateralus )
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include "sdl/playwave.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <X11/Xlocale.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/extensions/Xrandr.h>
#include "xdoutils.h"

#define INNER_WINDOW_WIDTH 50
#define INNER_WINDOW_HEIGHT 50
#define INNER_WINDOW_BORDER 4
#define INNER_WINDOW_X 10
#define INNER_WINDOW_Y 10
#define OUTER_WINDOW_MIN_WIDTH (INNER_WINDOW_WIDTH + \
				2 * (INNER_WINDOW_BORDER + INNER_WINDOW_X))
#define OUTER_WINDOW_MIN_HEIGHT (INNER_WINDOW_HEIGHT + \
				2 * (INNER_WINDOW_BORDER + INNER_WINDOW_Y))
#define OUTER_WINDOW_DEF_WIDTH (OUTER_WINDOW_MIN_WIDTH + 100)
#define OUTER_WINDOW_DEF_HEIGHT (OUTER_WINDOW_MIN_HEIGHT + 100)
#define OUTER_WINDOW_DEF_X 100
#define OUTER_WINDOW_DEF_Y 100

typedef unsigned long Pixel;

const char *Yes = "YES";
const char *No = "NO";
const char *Unknown = "unknown";

const char *ProgramName;
Display *dpy;
int screen;
Window main_window;

XIC xic = NULL;

Atom wm_delete_window;
Atom wm_protocols;

Bool have_rr;
int rr_event_base, rr_error_base;

enum EventMaskIndex {
    EVENT_MASK_INDEX_CORE,
    EVENT_MASK_INDEX_RANDR,
    NUM_EVENT_MASKS
};

static void
do_MotionNotify (XEvent *eventp)
{
    //XMotionEvent *e = (XMotionEvent *) eventp;
//    maximize_window(dpy,main_window);
    play_wave((const char*)"alarm.wav");
}
/*
*/
static void
set_sizehints (XSizeHints *hintp, int min_width, int min_height,
	       int defwidth, int defheight, int defx, int defy,
	       char *geom)
{
    int geom_result;

    /* set the size hints, algorithm from xlib xbiff */

    hintp->width = hintp->min_width = min_width;
    hintp->height = hintp->min_height = min_height;
    hintp->flags = PMinSize;
    hintp->x = hintp->y = 0;
    geom_result = NoValue;
    if (geom != NULL) {
        geom_result = XParseGeometry (geom, &hintp->x, &hintp->y,
				      (unsigned int *)&hintp->width,
				      (unsigned int *)&hintp->height);
	if ((geom_result & WidthValue) && (geom_result & HeightValue)) {
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
	    hintp->width = max (hintp->width, hintp->min_width);
	    hintp->height = max (hintp->height, hintp->min_height);
	    hintp->flags |= USSize;
	}
	if ((geom_result & XValue) && (geom_result & YValue)) {
	    hintp->flags += USPosition;
	}
    }
    if (!(hintp->flags & USSize)) {
	hintp->width = defwidth;
	hintp->height = defheight;
	hintp->flags |= PSize;
    }

    if (!(hintp->flags & USPosition)) {
	hintp->x = defx;
	hintp->y = defy;
	hintp->flags |= PPosition;
    }

    if (geom_result & XNegative) {
	hintp->x = DisplayWidth (dpy, DefaultScreen (dpy)) + hintp->x -
		    hintp->width;
    }
    if (geom_result & YNegative) {
	hintp->y = DisplayHeight (dpy, DefaultScreen (dpy)) + hintp->y -
		    hintp->height;
    }
}
static Bool
parse_event_mask (const char *s, long event_masks[])
{
    const struct {
        const char *name;
        enum EventMaskIndex mask_index;
        long mask;
    } events[] = {
        { "keyboard",
          EVENT_MASK_INDEX_CORE,
          KeyPressMask | KeyReleaseMask | KeymapStateMask },
        { "mouse",
          EVENT_MASK_INDEX_CORE,
          ButtonPressMask | ButtonReleaseMask | EnterWindowMask |
          LeaveWindowMask | PointerMotionMask | Button1MotionMask |
          Button2MotionMask | Button3MotionMask | Button4MotionMask |
          Button5MotionMask | ButtonMotionMask },
        { "expose",
          EVENT_MASK_INDEX_CORE,
          ExposureMask },
        { "visibility",
          EVENT_MASK_INDEX_CORE,
          VisibilityChangeMask },
        { "structure",
          EVENT_MASK_INDEX_CORE,
          StructureNotifyMask },
        { "substructure",
          EVENT_MASK_INDEX_CORE,
          SubstructureNotifyMask | SubstructureRedirectMask },
        { "focus",
          EVENT_MASK_INDEX_CORE,
          FocusChangeMask },
        { "property",
          EVENT_MASK_INDEX_CORE,
          PropertyChangeMask },
        { "colormap",
          EVENT_MASK_INDEX_CORE,
          ColormapChangeMask },
        { "owner_grab_button",
          EVENT_MASK_INDEX_CORE,
          OwnerGrabButtonMask },
        { "randr",
          EVENT_MASK_INDEX_RANDR,
          RRScreenChangeNotifyMask | RRCrtcChangeNotifyMask |
          RROutputChangeNotifyMask | RROutputPropertyNotifyMask },
        { NULL, (EventMaskIndex)0, 0 }
    };
    int i;

    for (i = 0; events[i].name; i++) {
        if (!s || !strcmp(s, events[i].name)) {
            event_masks[events[i].mask_index] |= events[i].mask;
            if (s)
                return True;
        }
    }

    return False;
}


int
main (int argc, char **argv)
{
    char *displayname = NULL;
    char *geom = NULL;
    int i;
    XSizeHints hints;
    int borderwidth = 2;
    Window w, subw;
    XSetWindowAttributes attr;
    XWindowAttributes wattr;
    unsigned long mask = 0L;
    int done;
    const char *name = "Event Tester";
    Bool reverse = False;
    Bool use_root = False;
    unsigned long back, fore;
    XIM xim;
    XIMStyles *xim_styles;
    XIMStyle xim_style = 0;
    char *modifiers;
    char *imvalret;
    long event_masks[NUM_EVENT_MASKS];
    Bool event_mask_specified = False;

    ProgramName = argv[0];

    if (setlocale(LC_ALL,"") == NULL) {
	fprintf(stderr, "%s: warning: could not set default locale\n",
		ProgramName);
    }

    memset(event_masks, 0, sizeof(event_masks));

    w = 0;
/*    for (i = 1; i < argc; i++) { */
/*	char *arg = argv[i]; */

    displayname = ":0";

    /* if no -event options were specified, pretend all of them were */
    if (!event_mask_specified){
        parse_event_mask (NULL, event_masks);
        printf("Event mask specified. stub call to parse_event_mask\n");
    }

    dpy = XOpenDisplay (displayname);
    if (!dpy) {
	fprintf (stderr, "%s:  unable to open display '%s'\n",
		 ProgramName, XDisplayName (displayname));
	exit (1);
    }

    /* we're testing the default input method */
    modifiers = XSetLocaleModifiers ("@im=none");
    if (modifiers == NULL) {
        fprintf (stderr, "%s:  XSetLocaleModifiers failed\n", ProgramName);
    }

    xim = XOpenIM (dpy, NULL, NULL, NULL);
    if (xim == NULL) {
        fprintf (stderr, "%s:  XOpenIM failed\n", ProgramName);
    }

    if (xim) {
        imvalret = XGetIMValues (xim, XNQueryInputStyle, &xim_styles, NULL);
        if (imvalret != NULL || xim_styles == NULL) {
            fprintf (stderr, "%s:  input method doesn't support any styles\n", ProgramName);
        }

        if (xim_styles) {
            xim_style = 0;
            for (i = 0;  i < xim_styles->count_styles;  i++) {
                if (xim_styles->supported_styles[i] ==
                    (XIMPreeditNothing | XIMStatusNothing)) {
                    xim_style = xim_styles->supported_styles[i];
                    break;
                }
            }

            if (xim_style == 0) {
                fprintf (stderr, "%s: input method doesn't support the style we support\n",
                         ProgramName);
            }
            XFree (xim_styles);
        }
    }

    screen = DefaultScreen (dpy);

    attr.event_mask = event_masks[EVENT_MASK_INDEX_CORE];

    if (use_root)
	w = RootWindow(dpy, screen);

    if (w) {
	XGetWindowAttributes(dpy, w, &wattr);
	if (wattr.all_event_masks & ButtonPressMask)
	    attr.event_mask &= ~ButtonPressMask;
	attr.event_mask &= ~SubstructureRedirectMask;
	XSelectInput(dpy, w, attr.event_mask);
    } else 
        printf("Set sizehints\n");
	set_sizehints (&hints, OUTER_WINDOW_MIN_WIDTH, OUTER_WINDOW_MIN_HEIGHT,
		       OUTER_WINDOW_DEF_WIDTH, OUTER_WINDOW_DEF_HEIGHT,
		       OUTER_WINDOW_DEF_X, OUTER_WINDOW_DEF_Y, geom);
    
	if (reverse) {
	    back = BlackPixel(dpy,screen);
	    fore = WhitePixel(dpy,screen);
	} else {
	    back = WhitePixel(dpy,screen);
	    fore = BlackPixel(dpy,screen);
	}

	attr.background_pixel = back;
	attr.border_pixel = fore;
	mask |= (CWBackPixel | CWBorderPixel | CWEventMask);

	w = XCreateWindow (dpy, RootWindow (dpy, screen), hints.x, hints.y,
			   hints.width, hints.height, borderwidth, 0,
			   InputOutput, (Visual *)CopyFromParent,
			   mask, &attr);
    

	XSetStandardProperties (dpy, w, name, NULL, (Pixmap) 0,
				argv, argc, &hints);

	subw = XCreateSimpleWindow (dpy, w, INNER_WINDOW_X, INNER_WINDOW_Y,
				    INNER_WINDOW_WIDTH, INNER_WINDOW_HEIGHT,
				    INNER_WINDOW_BORDER,
				    attr.border_pixel, attr.background_pixel);

        wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
        wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(dpy, w, &wm_delete_window, 1);

	XMapWindow (dpy, subw);		/* map before w so that it appears */
	XMapWindow (dpy, w);

	printf ("Outer window is 0x%lx, inner window is 0x%lx\n", w, subw);
    //}

    if (xim && xim_style) {
        xic = XCreateIC (xim,
                         XNInputStyle, xim_style,
                         XNClientWindow, w,
                         XNFocusWindow, w,
                         NULL);

        if (xic == NULL) {
            fprintf (stderr, "XCreateIC failed\n");
        }
    }

    have_rr = XRRQueryExtension (dpy, &rr_event_base, &rr_error_base);
    if (have_rr) {
        int rr_major, rr_minor;

        if (XRRQueryVersion (dpy, &rr_major, &rr_minor)) {
            int rr_mask = event_masks[EVENT_MASK_INDEX_RANDR];

            if (rr_major == 1 && rr_minor <= 1) {
                rr_mask &= ~(RRCrtcChangeNotifyMask |
                             RROutputChangeNotifyMask |
                             RROutputPropertyNotifyMask);
            }

            XRRSelectInput (dpy, w, rr_mask);
        }
    }

    /* Maximize the window */
    maximize_window(dpy,w);
    for (done = 0; !done; ) {
        XEvent event;

        XNextEvent (dpy, &event);

        switch (event.type) {
          case MotionNotify:
            /* prologue (&event, "MotionNotify"); */
            do_MotionNotify (&event);
            break;
        }
        fflush(stdout);
    }

    XCloseDisplay (dpy);
    return  0;
}
