#include <stdio.h>
#include <stdlib.h>
#include <xdo.h>

void usage(void){
    printf("./a.out <display>\n");
    return;
}
int main(int argc,char** argv){
    Display* xdisp;
    const char* display_string;
    int ret = 0;
    xdo_t* xdo_ptr;
    if( !(argc - 1) ){
        usage();
        return 0;
    }else{
        display_string = argv[1];
    }
    xdo_ptr = xdo_new(display_string);
    if( xdo_ptr == NULL ){
        fprintf(stderr,"xdo_new failed\n");
        return -1;
    }
    unsigned int xint,yint,seed;
    seed = atoi(argv[2]);
    srand(seed);
    while(1){
        xint = rand() / 1000000;
        yint = rand() / 1000000;
        printf("X: %d Y: %d\n",xint,yint);
        xdo_move_mouse(xdo_ptr,xint,yint,0);
    }

    return 0;
}
