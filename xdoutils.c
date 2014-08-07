#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <xdo.h>

int move_mouse_to(unsigned int xint,unsigned int yint,const char const * display_string){
    xdo_t* xdo_ptr;
    xdo_ptr = xdo_new(display_string);
    if( xdo_ptr == NULL ){
        fprintf(stderr,"xdo_new failed\n");
        return -1;
    }
    xdo_move_mouse(xdo_ptr,xint,yint,0);

    return 0;
}



// -----------------------------------------------------------
// Purpose : simply retrieve current X screen resolution and 
//    the size of current root window of the default 
//    screen of curent default window
// -----------------------------------------------------------
// Code taken from: http://surfingtroves.blogspot.nl/2011/01/how-to-get-screen-resolution-in-linux-c.html
//

int getRootWindowSize(int *w, int *h)
{
 Display* pdsp = NULL;
 Window wid = 0;
 XWindowAttributes xwAttr;

 pdsp = XOpenDisplay( NULL );
 if ( !pdsp ) {
  fprintf(stderr, "Failed to open default display.\n");
  return -1;
 }

 wid = DefaultRootWindow( pdsp );
 if ( 0 > wid ) {
  fprintf(stderr, "Failed to obtain the root windows Id "
      "of the default screen of given display.\n");
  return -2;
 }
 
 //Status ret = XGetWindowAttributes( pdsp, wid, &xwAttr );
 *w = xwAttr.width;
 *h = xwAttr.height;

 XCloseDisplay( pdsp );
 return 0;
}


// Code taken from: http://surfingtroves.blogspot.nl/2011/01/how-to-get-screen-resolution-in-linux-c.html

int getScreenSize(int *w, int*h)
{

 Display* pdsp = NULL;
 Screen* pscr = NULL;

 pdsp = XOpenDisplay( NULL );
 if ( !pdsp ) {
  fprintf(stderr, "Failed to open default display.\n");
  return -1;
 }

    pscr = DefaultScreenOfDisplay( pdsp );
 if ( !pscr ) {
  fprintf(stderr, "Failed to obtain the default screen of given display.\n");
  return -2;
 }

 *w = pscr->width;
 *h = pscr->height;

 XCloseDisplay( pdsp );
 return 0;
}

void center_mouse(){
    int xlen = 0, ylen = 0;
    getScreenSize(&xlen,&ylen);
    move_mouse_to( xlen / 2, ylen / 2, ":0" );
}

enum
{
    _NET_WM_STATE_REMOVE =0,
    _NET_WM_STATE_ADD = 1,
    _NET_WM_STATE_TOGGLE =2
};

void maximize_window(Display* dpy,Window w){
//    XEvent xev;
    int xlen=0 , ylen = 0;
//    Atom wm_state  =  XInternAtom(dpy, "_NET_WM_STATE", False);
    Atom max_horz  =  XInternAtom(dpy, "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    Atom max_vert  =  XInternAtom(dpy, "_NET_WM_STATE_MAXIMIZED_VERT", False);

    printf("Max horz %d Max vert %d\n",max_horz,max_vert);
    getScreenSize(&xlen,&ylen);
    printf("Max X: %d Max Y: %d\n",xlen,ylen);
    /*
     * memset(&xev, 0, sizeof(xev));
    
    xev.type = ClientMessage;
    xev.xclient.window = w;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = _NET_WM_STATE_ADD;
    xev.xclient.data.l[1] = xlen;   //max_horz;
    xev.xclient.data.l[2] = ylen;   //max_vert;

    XSendEvent(dpy, w, False, SubstructureNotifyMask,&xev);
    */

    XResizeWindow(dpy,w,xlen,ylen);
}

 /*
 main(){
    center_mouse();
}*/
