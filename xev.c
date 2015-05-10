/*
 * Author: IteratedLateralus ( http://github.com/iteratedlateralus )
 */

#include "sdl/playwave.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <curl/curl.h>

extern char* optarg;
Display* dpy;
Window w;
char trigger_type[10];    
time_t grace_time;
time_t trigger_grace_time = 0;
int sound_off_count = 1;
char http_get_url[256];
int sound_off_threshhold = 20;
int sound_off_counter = 0;
int max_children = 10;
int child_count = 0;
int http_counter = 0;
int http_threshhold = 5;
int first_hit = 0; 

void usage(){
    printf("Usage:\n");
    printf("======\n");
    printf(" ./tripwire -t <type> [options]\n");
    printf("OPTIONS\n");
    printf("=======\n");
    printf(" -t <type>      Trigger type. One of: http,sms,sendmail,none\n");
    printf(" HTTP type\n");
    printf(" -u <url>       URL to hit\n");
    printf("OTHER OPTIONS\n");
    printf(" -x <width>     Width of the capture window\n");
    printf(" -y <height>    Height of the capture window\n");
    printf(" -s <count>     How many times to trigger alarm.wav\n");
    printf(" -T <thresh>    Soundoff threshhold\n");
    printf("EXAMPLES\n");
    printf("========\n");
    printf(" Using a window as input, calling a URL as the trigger type: \n");
    printf(" $ ./xev -t http -u http://example.com/script.php\n");
    printf("\n");
    printf(" Using /dev/input mice as input: \n");
    printf(" $ sudo cat /dev/input/mice | ./xev -d -t http -u http://example.com/script.php\n");
    printf("\n");
}

int grace_time_reached(){
    return time(NULL) >= grace_time;
}
int trigger_grace_time_reached(){
    return time(NULL) >= trigger_grace_time;
}
void trigger_sendmail(){
    

}

void trigger_http_get(){
    CURL* ch = curl_easy_init();
    if( ch == NULL ){
        fprintf(stderr,"Invalid curl handle returned\n");
        return;
    }
    curl_easy_setopt(ch,CURLOPT_URL,http_get_url);
    curl_easy_perform(ch);
    curl_easy_cleanup(ch);
}

void motionNotify (){

    if( sound_off_counter++ >= sound_off_threshhold ){
        if( strcmp(trigger_type,"sendmail") == 0 ){
            printf("Sendmail stub\n");
        }else if( strcmp(trigger_type,"sms") == 0 ){
            printf("SMS stub\n");
        }else if( strcmp(trigger_type,"http") == 0 ){
            if( http_counter++ >= http_threshhold || first_hit == 0 ){
                printf("HTTP stub\n");
                printf("Hitting URL: %s\n",http_get_url);
                trigger_http_get();
                http_counter = 0;
                first_hit++;
            }
        }else if( strcmp(trigger_type,"none") == 0 ){
            printf(".");
        }
        play_wave((const char*)"alarm.wav");
        trigger_grace_time = time(NULL) + 20;
        sound_off_counter = 0;

    }
}


int main(int argc,char** argv){
    XEvent e;
    int s,x,y,misc;
    int stdin_read = 0;
    char c;
    
    grace_time = time(NULL) + 10;
    
    while((c=getopt(argc,argv,"t:di:p:h:g:x:y:u:T:m:")) != -1){
        switch(c){
            case 't':
                strncpy(trigger_type,optarg,10);
                break;
            case 'd':
                stdin_read = 1;
                break;
            case 'x':
                x = atoi(optarg);
                break;
            case 'y':
                y = atoi(optarg);
                break;
            case 's':
                sound_off_count = atoi(optarg);
                break;
            case 'u':
                strncpy(http_get_url,optarg,256);
                break;
            case 'T':
                sound_off_threshhold = atoi(optarg);
                break;
            case 'm':
                max_children = atoi(optarg);
                break;
            case '?':
                fprintf(stderr,"Unknown option encountered\n");
                return -1;
        }
    }

    if( stdin_read ){
        while(1){
            misc = read(0,&c,sizeof(char));
            if( misc ){
                motionNotify();
            }
        }
    }else{
            dpy = XOpenDisplay(NULL);
            if( dpy == NULL ){
                fprintf(stderr,"Invalid display\n");
                return 0;
            }


            s = DefaultScreen(dpy);
            w = XCreateSimpleWindow(dpy, RootWindow(dpy,s),
                10,10,x,y,1,
                BlackPixel(dpy,s),
                WhitePixel(dpy,s)
            );
            
            XSelectInput( dpy, w, ExposureMask | KeyPressMask | PointerMotionMask );
            XMapWindow( dpy, w );
            while(1){
                XNextEvent(dpy,&e);
                if( e.type == MotionNotify ){
                    motionNotify();
                }
            }
    }
    return 0;
}
