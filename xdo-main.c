#include <stdio.h>
#include <stdlib.h>
#include <xdo.h>

void usage(void){
    printf("./a.out <display>\n");
    return;
}
int move_mouse_to(unsigned xint,unsigned yint,const char const * display_string){
    Display* xdisp;
    int ret = 0;
    xdo_t* xdo_ptr;
    xdo_ptr = xdo_new(display_string);
    if( xdo_ptr == NULL ){
        fprintf(stderr,"xdo_new failed\n");
        return -1;
    }
    xdo_move_mouse(xdo_ptr,xint,yint,0);

    return 0;
}
