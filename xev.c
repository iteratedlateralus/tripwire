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
#include "aplay.h"
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

/*
static void usage (void) _X_NORETURN;
*/

/*
static void
prologue (XEvent *eventp, const char *event_name)
{
    XAnyEvent *e = (XAnyEvent *) eventp;

    printf ("\n%s event, serial %ld, synthetic %s, window 0x%lx,\n",
	    event_name, e->serial, e->send_event ? Yes : No, e->window);
}
*/
/*
static void
dump (char *str, int len)
{
    printf("(");
    len--;
    while (len-- > 0)
        printf("%02x ", (unsigned char) *str++);
    printf("%02x)", (unsigned char) *str++);
}
*/
/*
static void
do_KeyPress (XEvent *eventp)
{
    XKeyEvent *e = (XKeyEvent *) eventp;
    KeySym ks;
    KeyCode kc = 0;
    Bool kc_set = False;
    const char *ksname;
    int nbytes, nmbbytes = 0;
    char str[256+1];
    static char *buf = NULL;
    static int bsize = 8;
    Status status;

    if (buf == NULL)
      buf = malloc (bsize);

    nbytes = XLookupString (e, str, 256, &ks, NULL);
*/
    /* not supposed to call XmbLookupString on a key release event */
/*    if (e->type == KeyPress && xic) {
        do {
            nmbbytes = XmbLookupString (xic, e, buf, bsize - 1, &ks, &status);
            buf[nmbbytes] = '\0';

            if (status == XBufferOverflow) {
                bsize = nmbbytes + 1;
                buf = realloc (buf, bsize);
            }
        } while (status == XBufferOverflow);
    }

    if (ks == NoSymbol)
	ksname = "NoSymbol";
    else {
	if (!(ksname = XKeysymToString (ks)))
	    ksname = "(no name)";
	kc = XKeysymToKeycode(dpy, ks);
	kc_set = True;
    }

    printf ("    root 0x%lx, subw 0x%lx, time %lu, (%d,%d), root:(%d,%d),\n",
	    e->root, e->subwindow, e->time, e->x, e->y, e->x_root, e->y_root);
    printf ("    state 0x%x, keycode %u (keysym 0x%lx, %s), same_screen %s,\n",
	    e->state, e->keycode, (unsigned long) ks, ksname,
	    e->same_screen ? Yes : No);
    if (kc_set && e->keycode != kc)
	printf ("    XKeysymToKeycode returns keycode: %u\n",kc);
    if (nbytes < 0) nbytes = 0;
    if (nbytes > 256) nbytes = 256;
    str[nbytes] = '\0';
    printf ("    XLookupString gives %d bytes: ", nbytes);
    if (nbytes > 0) {
        dump (str, nbytes);
        printf (" \"%s\"\n", str);
    } else {
    	printf ("\n");
    }
*/
    /* not supposed to call XmbLookupString on a key release event */
/*    if (e->type == KeyPress && xic) {
        printf ("    XmbLookupString gives %d bytes: ", nmbbytes);
        if (nmbbytes > 0) {
           dump (buf, nmbbytes);
           printf (" \"%s\"\n", buf);
        } else {
    	   printf ("\n");
        }
    }

    printf ("    XFilterEvent returns: %s\n",
	    XFilterEvent (eventp, e->window) ? "True" : "False");
}*/
/*
static void
do_KeyRelease (XEvent *eventp)
{
    do_KeyPress (eventp);*/		/* since it has the same info */
/* }
 */
/*
static void
do_ButtonPress (XEvent *eventp)
{
    XButtonEvent *e = (XButtonEvent *) eventp;

    printf ("    root 0x%lx, subw 0x%lx, time %lu, (%d,%d), root:(%d,%d),\n",
	    e->root, e->subwindow, e->time, e->x, e->y, e->x_root, e->y_root);
    printf ("    state 0x%x, button %u, same_screen %s\n",
	    e->state, e->button, e->same_screen ? Yes : No);
}
*/
/*
static void
do_ButtonRelease (XEvent *eventp)
{
    do_ButtonPress (eventp);
    */		/* since it has the same info */
/*
 * }
*/
static void
do_MotionNotify (XEvent *eventp)
{
    //XMotionEvent *e = (XMotionEvent *) eventp;

    /*
    printf ("    root 0x%lx, subw 0x%lx, time %lu, (%d,%d), root:(%d,%d),\n",
	    e->root, e->subwindow, e->time, e->x, e->y, e->x_root, e->y_root);
    printf ("    state 0x%x, is_hint %u, same_screen %s\n",
	    e->state, e->is_hint, e->same_screen ? Yes : No);
    */
    maximize_window(dpy,main_window);
    play_wave("alarm.wav");
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
/*
static void
usage (void)
{
    static const char *msg[] = {
"    -display displayname                X server to contact",
"    -geometry geom                      size and location of window",
"    -bw pixels                          border width in pixels",
"    -bs {NotUseful,WhenMapped,Always}   backingstore attribute",
"    -id windowid                        use existing window",
"    -root                               use root window",
"    -s                                  set save-unders attribute",
"    -name string                        window name",
"    -rv                                 reverse video",
"    -event event_mask                   select 'event_mask' events",
"           Supported event masks: keyboard mouse expose visibility structure",
"                                  substructure focus property colormap",
"                                  owner_grab_button randr",
"           This option can be specified multiple times to select multiple",
"           event masks.",
"",
NULL};
    const char **cpp;

    fprintf (stderr, "usage:  %s [-options ...]\n", ProgramName);
    fprintf (stderr, "where options include:\n");

    for (cpp = msg; *cpp; cpp++)
	fprintf (stderr, "%s\n", *cpp);

    exit (1);
}

static int
parse_backing_store (char *s)
{
    size_t len = strlen (s);

    if (strncasecmp (s, "NotUseful", len) == 0) return (NotUseful);
    if (strncasecmp (s, "WhenMapped", len) == 0) return (WhenMapped);
    if (strncasecmp (s, "Always", len) == 0) return (Always);

    usage ();
}
*/
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
        { NULL, 0, 0 }
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

	/*if (arg[0] == '-') {
	    switch (arg[1]) {
	      case 'd':*/			/* -display host:dpy */
/*		if (++i >= argc) usage ();
		displayname = argv[i];
		continue;
	      case 'g':*/			/* -geometry geom */
/*		if (++i >= argc) usage ();
		geom = argv[i];
		continue;
	      case 'b':
		switch (arg[2]) {
		  case 'w':*/		/* -bw pixels */
/*		    if (++i >= argc) usage ();
		    borderwidth = atoi (argv[i]);
		    continue;
		  case 's':	*/	/* -bs type */
/*		    if (++i >= argc) usage ();
		    attr.backing_store = parse_backing_store (argv[i]);
		    mask |= CWBackingStore;
		    continue;
		  default:
		    usage ();
		}
	      case 'i':*/			/* -id */
/*		if (++i >= argc) usage ();
		sscanf(argv[i], "0x%lx", &w);
		if (!w)
		    sscanf(argv[i], "%lu", &w);
		if (!w)
		    usage ();
		continue;
	      case 'n':	*/		/* -name */
/*		if (++i >= argc) usage ();
		name = argv[i];
		continue;
	      case 'r':
		switch (arg[2]) {
		  case 'o':*/		/* -root */
/*		    use_root = True;
		    continue;
		  case 'v':*/		/* -rv */
/*		    reverse = True;
		    continue;
		  default:
		    usage ();
		}
		continue;
	      case 's':*/			/* -s */
/*		attr.save_under = True;
		mask |= CWSaveUnder;
		continue;
	      case 'e':*/			/* -event */
/*		if (++i >= argc) usage ();
		if (!parse_event_mask (argv[i], event_masks))
		    usage ();
		event_mask_specified = True;
		continue;
	      default:
		usage ();
	    }*/				/* end switch on - */
/*	} else
	  usage ();
    }*/					/* end for over argc */
    
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
        /*
	  case KeyPress:
	    prologue (&event, "KeyPress");
	    do_KeyPress (&event);
	    break;
	  case KeyRelease:
	    prologue (&event, "KeyRelease");
	    do_KeyRelease (&event);
	    break;
	  case ButtonPress:
	    prologue (&event, "ButtonPress");
	    do_ButtonPress (&event);
	    break;
	  case ButtonRelease:
	    prologue (&event, "ButtonRelease");
	    do_ButtonRelease (&event);
	    break;
        */
	  case MotionNotify:
	    /* prologue (&event, "MotionNotify"); */
	    do_MotionNotify (&event);
	    break;
      /*
	  case EnterNotify:
	    prologue (&event, "EnterNotify");
	    do_EnterNotify (&event);
	    break;
	  case LeaveNotify:
	    prologue (&event, "LeaveNotify");
	    do_LeaveNotify (&event);
	    break;
	  case FocusIn:
	    prologue (&event, "FocusIn");
	    do_FocusIn (&event);
	    break;
	  case FocusOut:
	    prologue (&event, "FocusOut");
	    do_FocusOut (&event);
	    break;
	  case KeymapNotify:
	    prologue (&event, "KeymapNotify");
	    do_KeymapNotify (&event);
	    break;
	  case Expose:
	    prologue (&event, "Expose");
	    do_Expose (&event);
	    break;
	  case GraphicsExpose:
	    prologue (&event, "GraphicsExpose");
	    do_GraphicsExpose (&event);
	    break;
	  case NoExpose:
	    prologue (&event, "NoExpose");
	    do_NoExpose (&event);
	    break;
	  case VisibilityNotify:
	    prologue (&event, "VisibilityNotify");
	    do_VisibilityNotify (&event);
	    break;
	  case CreateNotify:
	    prologue (&event, "CreateNotify");
	    do_CreateNotify (&event);
	    break;
	  case DestroyNotify:
	    prologue (&event, "DestroyNotify");
	    do_DestroyNotify (&event);
	    break;
	  case UnmapNotify:
	    prologue (&event, "UnmapNotify");
	    do_UnmapNotify (&event);
	    break;
	  case MapNotify:
	    prologue (&event, "MapNotify");
	    do_MapNotify (&event);
	    break;
	  case MapRequest:
	    prologue (&event, "MapRequest");
	    do_MapRequest (&event);
	    break;
	  case ReparentNotify:
	    prologue (&event, "ReparentNotify");
	    do_ReparentNotify (&event);
	    break;
	  case ConfigureNotify:
	    prologue (&event, "ConfigureNotify");
	    do_ConfigureNotify (&event);
	    break;
	  case ConfigureRequest:
	    prologue (&event, "ConfigureRequest");
	    do_ConfigureRequest (&event);
	    break;
	  case GravityNotify:
	    prologue (&event, "GravityNotify");
	    do_GravityNotify (&event);
	    break;
	  case ResizeRequest:
	    prologue (&event, "ResizeRequest");
	    do_ResizeRequest (&event);
	    break;
	  case CirculateNotify:
	    prologue (&event, "CirculateNotify");
	    do_CirculateNotify (&event);
	    break;
	  case CirculateRequest:
	    prologue (&event, "CirculateRequest");
	    do_CirculateRequest (&event);
	    break;
	  case PropertyNotify:
	    prologue (&event, "PropertyNotify");
	    do_PropertyNotify (&event);
	    break;
	  case SelectionClear:
	    prologue (&event, "SelectionClear");
	    do_SelectionClear (&event);
	    break;
	  case SelectionRequest:
	    prologue (&event, "SelectionRequest");
	    do_SelectionRequest (&event);
	    break;
	  case SelectionNotify:
	    prologue (&event, "SelectionNotify");
	    do_SelectionNotify (&event);
	    break;
	  case ColormapNotify:
	    prologue (&event, "ColormapNotify");
	    do_ColormapNotify (&event);
	    break;
	  case ClientMessage:
	    prologue (&event, "ClientMessage");
	    do_ClientMessage (&event);
	    break;
	  case MappingNotify:
	    prologue (&event, "MappingNotify");
	    do_MappingNotify (&event);
	    break;
        */
      /*
	  default:
	    if (have_rr) {
	        if (event.type == rr_event_base + RRScreenChangeNotify) {
	            prologue (&event, "RRScreenChangeNotify");
	            do_RRScreenChangeNotify (&event);
	            break;
	        }
	        if (event.type == rr_event_base + RRNotify) {
	            do_RRNotify (&event);
	            break;
	        }
	    }
	    printf ("Unknown event type %d\n", event.type);
	    break;
        */
	}
	fflush(stdout);
    }

    XCloseDisplay (dpy);
    return  0;
}
